cmake_minimum_required(VERSION 3.8)
project(SurfaceScanner)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(image_transport REQUIRED)
find_package(OpenCV REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(pcl_ros REQUIRED)
find_package(interfaces REQUIRED)

add_executable(camera_node src/camera_node.cpp)
ament_target_dependencies(camera_node OpenCV interfaces
                          cv_bridge sensor_msgs rclcpp std_srvs)

add_executable(surface_scanner_node src/SurfaceScannerNode.cpp)
ament_target_dependencies(surface_scanner_node OpenCV interfaces
                          cv_bridge sensor_msgs rclcpp std_srvs std_msgs pcl_ros pcl_conversions)

install(TARGETS
  camera_node
  surface_scanner_node
  DESTINATION lib/${PROJECT_NAME})

ament_package()
