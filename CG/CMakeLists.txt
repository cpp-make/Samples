cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

project(pt VERSION 1.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 画像出力
add_executable(
	chapter_image
	chapter_image.cpp
	vec.h
	real.h
	utility.h
	utility.cpp
)

# 交差判定
add_executable(
	chapter_intersection
	chapter_intersection.cpp
	camera.h
	vec.h
	ray.h
	real.h
	utility.h
	utility.cpp
)

# 影とLambertのシンプルなシーン
add_executable(
	chapter_raytracing
	chapter_raytracing.cpp
	camera.h
	vec.h
	ray.h
	real.h
	utility.h
	utility.cpp
	scene.h
)

# 基本のパストレーシング
add_executable(
	chapter_pathtracing
	chapter_pathtracing.cpp
	vec.h
	ray.h
	math.h
	random_pcg.h
	random.h
	real.h
	sphere.h
	utility.h
	utility.cpp
	material.h
	scene.h
)

# 進んだパストレーシング(鏡、アンチエイリアシング、並列化)
add_executable(
	chapter_advanced_pathtracing
	chapter_advanced_pathtracing.cpp
	vec.h
	ray.h
	math.h
	random_pcg.h
	random.h
	real.h
	sphere.h
	utility.h
	utility.cpp
	material.h
	scene.h
)

# 図18: 球、マテリアル、カメラ構図を変更したシーン
add_executable(
	chapter_advanced_pathtracing_scene
	chapter_advanced_pathtracing_scene.cpp
	vec.h
	ray.h
	math.h
	random_pcg.h
	random.h
	real.h
	sphere.h
	utility.h
	utility.cpp
	material.h
	scene_figure18.h
)
