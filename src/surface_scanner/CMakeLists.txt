cmake_minimum_required(VERSION 3.8)
project(surface_scanner)

#Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

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
#find_package(serial REQUIRED)   #not used 

include_directories(include)

add_executable(camera_node src/camera_node.cpp)
ament_target_dependencies(camera_node OpenCV interfaces
                          cv_bridge sensor_msgs rclcpp std_srvs)

add_executable(surface_scanner_node src/surface_scanner_node.cpp)
ament_target_dependencies(surface_scanner_node OpenCV interfaces
                          cv_bridge sensor_msgs rclcpp std_srvs std_msgs pcl_ros pcl_conversions)

add_executable(calibrate src/calibration_client.cpp)
ament_target_dependencies(calibrate rclcpp std_srvs)

add_executable(calibrate_with_import src/calibration_client_import.cpp)
ament_target_dependencies(calibrate_with_import interfaces rclcpp std_srvs)

add_executable(start_img_stream src/start_img_stream.cpp)
ament_target_dependencies(start_img_stream rclcpp std_srvs)

add_executable(stop_img_stream src/stop_img_stream.cpp)
ament_target_dependencies(stop_img_stream rclcpp std_srvs)

add_executable(intr_calibration src/trigger_take_cam_calib_imgs.cpp)
ament_target_dependencies(intr_calibration rclcpp std_srvs)

add_executable(extr_calibration src/trigger_take_img_pair_calib.cpp)
ament_target_dependencies(extr_calibration rclcpp std_srvs)

add_executable(surface_line_imgs src/trigger_take_img_pair_surface.cpp)
ament_target_dependencies(surface_line_imgs rclcpp std_srvs)

install(
  DIRECTORY include/surface_scanner
  DESTINATION include
)

install(TARGETS
  camera_node
  surface_scanner_node
  calibrate
  calibrate_with_import
  start_img_stream
  stop_img_stream
  intr_calibration
  extr_calibration
  surface_line_imgs
  DESTINATION lib/${PROJECT_NAME}
  INCLUDES DESTINATION include)

ament_package()
