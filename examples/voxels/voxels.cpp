#include "graphics/GLcommon.h"

#include <cstdlib>
#include <cstdio>
#include <cmath>

#include "EntityManager.h"
#include "graphics/png/PngLoader.h"
#include "base/Timer.h"
#include "camera/StrategicCamera.h"
#include "camera/FreeFlyCamera.h"
#include "shaders/DefaultShader.h"
#include "shaders/FinalRenderShader.h"
#include "shaders/ShadowBufferShader.h"
#include "shaders/ShadowShader.h"
#include "shaders/DeferredShader.h"
#include "shaders/BlurShader.h"
#include "shaders/BlinnPhongShader.h"
#include "shaders/SSAOShader.h"
#include "graphics/VBOManager.h"
#include "scene/GraphicRenderer.h"
#include "graphics/RenderTarget.h"
#include "ScreenVertex.h"

#include "particles/ParticleSystem.h"

#include "base/frustumParams.h"
#include "voxels/VBOChunk.h"
#include "voxels/VoxelWorld.h"
#include "voxels/VoxelWordGenerator.h"

#include "mesh/OBJMeshLoader.h"
#include "mesh/MeshNode.h"

#include "base/impBase.h"
#include "base/State.h"

#include "sky/SkyBox.h"

#include "SFMLContextInterface.h"

#define FPS_LIMIT 60
#define IS_FULLSCREEN false

imp::FreeFlyCamera cam(WIN_W/2, WIN_H/2);
//imp::StrategicCamera cam;
imp::Camera sunCamera;
imp::VoxelWorld* world;

imp::Uint32 debugMode;

void onEvent(imp::EvnContextInterface& evnContext){

    evnContext.getEvents(0);

    imp::Event event;
    while (evnContext.nextEvent(event))
        imp::State::getInstance()->onEvent(event);

	imp::State* state = imp::State::getInstance();

	if(state->m_pressedKeys[imp::Event::Escape])
		exit(0);

	for(imp::Uint32 f=1; f<9; ++f)
	{
		if(state->m_pressedKeys[imp::Event::F1+(f-1)])
		{
			debugMode = f;
			break;
		}
	}
}

void loadCameraKeyBinding(const char* filename)
{
	imp::Parser file(filename, imp::Parser::FileType_Text, imp::Parser::AccessMode_Read);
	imp::KeyBindingConfig binding;
	binding.Load(&file);
	cam.setKeyBindingConfig(binding);
}

int main(void)
{

    imp::State state;

    imp::Timer timer, fpsTimer;
    int fps = 0, nbFrames = 0;
	debugMode = 1;

    imp::EvnContextInterface* evnContext = new imp::SFMLContextInterface();
    evnContext->createWindow(WIN_W, WIN_H);
    evnContext->setCursorVisible(0, false);

	// sun camera for shadow
    sunCamera.setPosition(imp::Vector3(150.f, 150.f, 300.f));
    sunCamera.setTarget(imp::Vector3(64.f, 64.f, 64.f));
    imp::GraphicRenderer renderer(0, &sunCamera);
    renderer.setCenterCursor(true);


    imp::MeshNode mesh(imp::OBJMeshLoader::loadFromFile("data/obj-models/simple-monkey.c.obj"));
    mesh.setPosition(imp::Vector3(100.f, 100.f, 80.f));


    /// atlas - color
    imp::Texture* atlasColor = PngLoader::loadFromFile("data/atlas-color.png");
    atlasColor->setSmooth(true);
    atlasColor->setRepeated(false);
    atlasColor->setMipmap(true, 9);
    atlasColor->synchronize();

    /// atlas - specular
    imp::Texture* atlasSpec = PngLoader::loadFromFile("data/atlas-specular.png");
    atlasSpec->setSmooth(true);
    atlasSpec->setRepeated(false);
    atlasSpec->setMipmap(true, 9);
    atlasSpec->synchronize();

    /// atlas - normal
    imp::Texture* atlasNormal = PngLoader::loadFromFile("data/atlas-normal.png");
    atlasNormal->setSmooth(true);
    atlasNormal->setRepeated(false);
    atlasNormal->setMipmap(true, 9);
    atlasNormal->synchronize();

    /// atlas - self illumination
    imp::Texture* atlasSelf = PngLoader::loadFromFile("data/atlas-selfillu.png");
    atlasSelf->setSmooth(true);
    atlasSelf->setRepeated(false);
    atlasSelf->setMipmap(true, 9);
    atlasSelf->synchronize();

    imp::EntityManager entityManager;

	// default render parameters
    imp::RenderParameters defaultParameters;
    defaultParameters.setPerspectiveProjection(FRUSTUM_FOVY, 4.f/3.f, FRUSTUM_NEAR, FRUSTUM_FAR);

	// screen render parameters
    imp::RenderParameters screenParameters;
    screenParameters.setOrthographicProjection(0.f, 1.f, 0.f, 1.f, 0.f, 1.f);

	// shadow shaders
    imp::ShadowBufferShader shadowBufferShader;
    imp::ShadowShader shadowShader;
    imp::RenderParameters shadowParameters;
    shadowParameters.setFaceCullingMode(imp::RenderParameters::FaceCullingMode_Back);
    shadowParameters.setOrthographicProjection(-130.f, 130.f, -130.f, 130.f, 0.1, 512.f);

	// world generation
    world = new imp::VoxelWorld(128,128,64);
	fprintf(stdout, "world generation...\t");
    imp::VoxelWordGenerator::GetInstance()->Generate(world);
	fprintf(stdout, "OK\n");
	fprintf(stdout, "update chunk vbo...\t");
    world->UpdateAll();
    fprintf(stdout, "gpu memory used : %dMo\n", imp::VBOManager::getInstance()->getMemoryUsed()/1000000);

    cam.initFrustum(WIN_W, WIN_H, FRUSTUM_FOVY*3.14159f/180.f, FRUSTUM_NEAR, FRUSTUM_FAR);
    entityManager.addEntity(&cam);
    sunCamera.initFrustum(WIN_W, WIN_H, FRUSTUM_FOVY*3.14159f/180.f, 0.1, 512.f);

	// render target for shadow sun depth buffer
    imp::RenderTarget shadowBuffer;
    shadowBuffer.createBufferTarget(WIN_W, WIN_H, 0, true);
    const imp::Texture* shadowBufferTex = shadowBuffer.getDepthTexture();

	// render target for shadows buffer
    imp::RenderTarget shadows;
    shadows.createBufferTarget(WIN_W, WIN_H, 1, false);
    const imp::Texture* shadowsTex = shadows.getTexture(0);

	// render target for background
    imp::RenderTarget backgroundTarget;
    backgroundTarget.createBufferTarget(WIN_W, WIN_H, 1, false);
    const imp::Texture* backgroundTex = backgroundTarget.getTexture(0);

	// render target for self-illumination
    imp::RenderTarget selfiTarget;
    selfiTarget.createBufferTarget(WIN_W/4.f, WIN_H/4.f, 1, false);
    const imp::Texture* selfiTex = selfiTarget.getTexture(0);

	// render target for light effect
    imp::RenderTarget blinnPhongTarget;
    blinnPhongTarget.createBufferTarget(WIN_W, WIN_H, 1, false);
    const imp::Texture* blinnPhongBuffer = blinnPhongTarget.getTexture(0);
    imp::BlinnPhongShader blinnPhong;

    imp::FinalRenderShader finalShader;

	// render target for deffered rendering
    imp::RenderTarget deferredBuffers;
    deferredBuffers.createBufferTarget(WIN_W, WIN_H, 4, true);
    const imp::Texture* colorBuffer = deferredBuffers.getTexture(0);
    const imp::Texture* normalBuffer = deferredBuffers.getTexture(1);
    imp::Texture* specBuffer = deferredBuffers.getTexture(2);
    const imp::Texture* selfBuffer = deferredBuffers.getTexture(3);
    const imp::Texture* depthBuffer = deferredBuffers.getDepthTexture();
    imp::DeferredShader deferredShader;

	// SSAO shader
	imp::SSAOShader ssaoShader;
    imp::RenderTarget ssaoBuffer;
    ssaoBuffer.createBufferTarget(WIN_W, WIN_H, 1, false);
    const imp::Texture* ssaoTex = ssaoBuffer.getTexture(0);

	// blur shader
    imp::BlurShader blurShader;
    imp::RenderTarget ssaoBlurTarget;
    ssaoBlurTarget.createBufferTarget(WIN_W, WIN_H, 1, false);
    const imp::Texture* ssaoBlurTex = ssaoBlurTarget.getTexture(0);

	// screen render target
    imp::RenderTarget screenTarget;
    screenTarget.createScreenTarget(0);

	// textures for skybox
    imp::Texture* skyLeft = PngLoader::loadFromFile("data/sky/box/left.png"); skyLeft->synchronize();
    imp::Texture* skyRight = PngLoader::loadFromFile("data/sky/box/right.png"); skyRight->synchronize();
    imp::Texture* skyFront = PngLoader::loadFromFile("data/sky/box/front.png"); skyFront->synchronize();
    imp::Texture* skyBack = PngLoader::loadFromFile("data/sky/box/back.png"); skyBack->synchronize();
    imp::Texture* skyTop = PngLoader::loadFromFile("data/sky/box/top.png"); skyTop->synchronize();
    imp::Texture* skyBottom = PngLoader::loadFromFile("data/sky/box/bottom.png"); skyBottom->synchronize();

	// skybox entity
	imp::SkyBox skyBox(skyLeft, skyRight, skyFront, skyBack, skyTop, skyBottom, FRUSTUM_FAR/2.f);
    renderer.getScene()->addSceneComponent(&skyBox);
    entityManager.addEntity(&skyBox);

    imp::DefaultShader defaultShader;

    float mapLevel = 128.f;

    imp::Matrix4 pMat;

    imp::ScreenVertex screen;

    imp::Matrix4 i4 = imp::Matrix4::getIdentityMat();


    const imp::Texture* texToDisplay = colorBuffer;

	// Local key binding
	imp::Uint32 m_levelDownKey = imp::Event::PageDown;
	imp::Uint32 m_levelUpKey = imp::Event::PageUp;

	// Camera key binding
	loadCameraKeyBinding("camera-key-binding.conf");

    while (evnContext->isOpen(0))
    {

		if( fpsTimer.getTime() >  (1000.f/FPS_LIMIT) )
		{

			onEvent(*imp::EvnContextInterface::getInstance());

            /// change level
            imp::State* state = imp::State::getInstance();
            if(state->m_pressedKeys[m_levelUpKey])
            {
                mapLevel += 0.5f;
                fprintf(stdout, "level : %f\n", mapLevel);
            }
            else if(state->m_pressedKeys[m_levelDownKey])
            {
                mapLevel -= 0.5f;
                fprintf(stdout, "level : %f\n", mapLevel);
            }

            entityManager.updateAll();
            //particles.update();

			nbFrames++;

            /// (pass 0) ShadowBuffer rendering /////////////////////
            renderer.setCamera(&sunCamera);
            renderer.setRenderParameters(shadowParameters);
            shadowBuffer.bind();
            shadowBufferShader.enable();
            shadowBufferShader.setTextureParameter("my_color_texture", atlasColor, 0);
            shadowBufferShader.setMatrix4Parameter("u_projection", renderer.getProjectionMatrix());
            shadowBufferShader.setMatrix4Parameter("u_view", imp::Camera::getActiveCamera()->getViewMatrix());
            renderer.renderScene(0);
            world->render(0);
            mesh.renderAll(0);
            shadowBufferShader.disable();
            shadowBuffer.unbind();
            /// ///////////////////////////////////////////

            /// (pass 3) Deffered shading ////////////
            pMat = renderer.getProjectionMatrix();
            renderer.setCamera(&cam);
            renderer.setRenderParameters(defaultParameters);
            deferredBuffers.bind();
            renderer.renderScene(-1);
            imp::Matrix4 vMat = imp::Camera::getActiveCamera()->getViewMatrix();
			deferredShader.enable();
            deferredShader.setMatrix4Parameter("u_projection", renderer.getProjectionMatrix());
            deferredShader.setMatrix4Parameter("u_view", vMat);
			deferredShader.setFloatParameter("u_mapLevel", mapLevel);
            deferredShader.setTextureParameter("u_colorTexture", atlasColor, 0);
            deferredShader.setTextureParameter("u_specTexture", atlasSpec, 1);
            deferredShader.setTextureParameter("u_normalTexture", atlasNormal, 2);
            deferredShader.setTextureParameter("u_selfTexture", atlasSelf, 3);
			deferredShader.setFloatParameter("u_far", FRUSTUM_FAR);
			world->render(3);
			mesh.renderAll(3);
			deferredShader.disable();
			deferredBuffers.unbind();
            /// ////////////////////////////////////////////////////


            /// (pass ?) Shadows
            renderer.setCamera(IMP_NULL);
            renderer.setRenderParameters(screenParameters);
            shadows.bind();
			shadowShader.enable();
            shadowShader.setMatrix4Parameter("u_projection", renderer.getProjectionMatrix());
            shadowShader.setMatrix4Parameter("u_shadowProjMat", shadowParameters.getProjectionMatrix());
            shadowShader.setMatrix4Parameter("u_shadowViewMat", sunCamera.getViewMatrix());
            shadowShader.setMatrix4Parameter("u_iviewMat", cam.getViewMatrix().getInverse());
            shadowShader.setTextureParameter("u_shadowBuffer", shadowBufferTex, 0);
            shadowShader.setTextureParameter("u_depthSampler", depthBuffer, 1);
            shadowShader.setFloatParameter("u_far", FRUSTUM_FAR);
            renderer.renderScene(-1);
            screen.render(0);
			shadowShader.disable();
			shadows.unbind();
            /// ////////////////////////////////////////////////////

            /// (pass ?) Background
            renderer.setCamera(&cam);
            renderer.setRenderParameters(defaultParameters);
            backgroundTarget.bind();
            renderer.renderScene(1);
			backgroundTarget.unbind();
            /// ////////////////////////////////////////////////////

			/// (pass 2) Screen Space Ambient Occlusion ////////////
            renderer.setCamera(IMP_NULL);
            renderer.setRenderParameters(screenParameters);
            ssaoBuffer.bind();
			ssaoShader.enable();
            ssaoShader.setMatrix4Parameter("u_projection", renderer.getProjectionMatrix());
            ssaoShader.setMatrix4Parameter("pMat", defaultParameters.getProjectionMatrix());
            ssaoShader.setMatrix4Parameter("vMat", cam.getViewMatrix());
            ssaoShader.setVector3ArrayParameter("sampleKernel", ssaoShader.getSampleKernel(), IMP_SSAO_SAMPLE_KERNEL_SIZE);
            ssaoShader.setFloatParameter("u_sampleCount", 16.f);
			ssaoShader.setTextureParameter("depthTex", depthBuffer, 0);
			ssaoShader.setTextureParameter("normalTex", normalBuffer, 1);
			ssaoShader.setTextureParameter("noiseTex", ssaoShader.getNoiseTexture(), 2);
			ssaoShader.setFloatParameter("u_far", FRUSTUM_FAR);
			ssaoShader.setFloatParameter("u_near", FRUSTUM_NEAR);
            renderer.renderScene(-1);
            screen.render(0);
			ssaoShader.disable();
			ssaoTex = ssaoBuffer.getTexture(0);
			ssaoBuffer.unbind();
            /// ////////////////////////////////////////////////////

            /// (pass ?) ssao blur ////////////
            renderer.setCamera(IMP_NULL);
            renderer.setRenderParameters(screenParameters);
            ssaoBlurTarget.bind();
			blurShader.enable();
            blurShader.setMatrix4Parameter("u_projection", renderer.getProjectionMatrix());
            blurShader.setTextureParameter("u_colorTexture", ssaoTex, 0);
            blurShader.setFloatParameter("u_blurSize_x", 2.f);
            blurShader.setFloatParameter("u_blurSize_y", 2.f);
            renderer.renderScene(-1);
            screen.render(0);
			blurShader.disable();
			ssaoBlurTarget.unbind();
            /// ////////////////////////////////////////////////////

            /// (pass ?) blinn-phong model ////////////
            renderer.setCamera(IMP_NULL);
            renderer.setRenderParameters(screenParameters);
            blinnPhongTarget.bind();
			blinnPhong.enable();
            blinnPhong.setMatrix4Parameter("u_projection", renderer.getProjectionMatrix());
            blinnPhong.setTextureParameter("u_ambientIntensity", ssaoBlurTex, 0);
            blinnPhong.setTextureParameter("u_diffuseColor", colorBuffer, 1);
            blinnPhong.setTextureParameter("u_specularColor", specBuffer, 2);
            blinnPhong.setTextureParameter("u_normalSampler", normalBuffer, 3);
            blinnPhong.setTextureParameter("u_depthSampler", depthBuffer, 4);
            blinnPhong.setFloatParameter("u_far", FRUSTUM_FAR);
            blinnPhong.setVector3Parameter("u_viewSunPosition", cam.getViewMatrix() * sunCamera.getPosition());
            blinnPhong.setVector3Parameter("u_viewEyePosition", cam.getViewMatrix() * cam.getPosition());
            blinnPhong.setFloatParameter("u_intensity", 1.7f);
            blinnPhong.setFloatParameter("u_shininess", 64.f);
            renderer.renderScene(-1);
            screen.render(0);
			blinnPhong.disable();
			blinnPhongTarget.unbind();
            /// ////////////////////////////////////////////////////

            if(debugMode == 1)
            {
                /// (pass ?) Screen rendering
                renderer.setCamera(IMP_NULL);
                renderer.setRenderParameters(screenParameters);
                screenTarget.bind();
                finalShader.enable();
                finalShader.setMatrix4Parameter("u_projection", renderer.getProjectionMatrix());
                finalShader.setTextureParameter("u_colorTexture", blinnPhongBuffer, 0);
                finalShader.setTextureParameter("u_backgroundTexture", backgroundTex, 1);
                finalShader.setTextureParameter("u_selfTexture", selfBuffer, 2);
                finalShader.setTextureParameter("u_shadows", shadowsTex, 3);
                renderer.renderScene(-1);
                screen.render(0);
                finalShader.disable();
                screenTarget.unbind();
            }
            else
            {
                switch(debugMode)
                {
                    case 2:
                        texToDisplay = colorBuffer;
                    break;
                    case 3:
                        texToDisplay = ssaoBlurTex;
                    break;
                    case 4:
                        texToDisplay = shadowsTex;
                    break;
                    case 5:
                        texToDisplay = normalBuffer;
                    break;
                    case 6:
                        texToDisplay = depthBuffer;
                    break;
                    case 7:
                        texToDisplay = specBuffer;
                    break;
                    case 8:
                        texToDisplay = selfBuffer;
                    break;
                    case 9:
                        texToDisplay = blinnPhongBuffer;
                    break;
                    default:
                    break;
                }

                /// debug pass
                renderer.setCamera(IMP_NULL);
                renderer.setRenderParameters(screenParameters);
                screenTarget.bind();
                defaultShader.enable();
                defaultShader.setMatrix4Parameter("u_projection", renderer.getProjectionMatrix());
                defaultShader.setMatrix4Parameter("u_view", i4);
                defaultShader.setMatrix4Parameter("u_model", i4);
                defaultShader.setTextureParameter("u_colorTexture", texToDisplay, 0);
                renderer.renderScene(-1);
                screen.render(0);
                defaultShader.disable();
                screenTarget.unbind();
            }

            evnContext->display(0);

			fpsTimer.reset();
		}

        if(timer.getTime() >= 1000)
        {
        	fps = nbFrames/ (timer.getTime()/1000.f );
            nbFrames = 0;

            char buff[100];
            sprintf(buff, "fps : %d\n", fps);
            fprintf(stdout, buff);
            timer.reset();
        }
    }

    delete imp::VBOManager::getInstance();

    exit(EXIT_SUCCESS);
}
