cmake_minimum_required(VERSION 3.0)

set(SRC_NAME RTRenderer)
set(EXE_NAME BenchRT)
set(FOLDER_NAME src/benchs)
set(EXE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/${FOLDER_NAME}/${SRC_NAME})

set(SRCS
    ${PROJECT_SOURCE_DIR}/${FOLDER_NAME}/utils/AppContext.h
    ${PROJECT_SOURCE_DIR}/${FOLDER_NAME}/utils/inc_experimental.h
    ${EXE_SOURCE_DIR}/RTSceneRenderer.h
    ${EXE_SOURCE_DIR}/RTSceneRenderer.cpp
    ${EXE_SOURCE_DIR}/viewer.cpp
)

IG_SETUP_EXE(${EXE_NAME} ${EXE_SOURCE_DIR} "${SRCS}")