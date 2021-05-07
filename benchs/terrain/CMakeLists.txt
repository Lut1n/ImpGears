cmake_minimum_required(VERSION 3.0)

set(SRC_NAME terrain)
set(EXE_NAME BenchTerrain)
set(FOLDER_NAME benchs)
set(EXE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/${FOLDER_NAME}/${SRC_NAME})

set(SRCS
    ${PROJECT_SOURCE_DIR}/${FOLDER_NAME}/utils/AppContext.h
    ${EXE_SOURCE_DIR}/viewer.cpp
)

IG_SETUP_EXE(${EXE_NAME} ${EXE_SOURCE_DIR} "${SRCS}")