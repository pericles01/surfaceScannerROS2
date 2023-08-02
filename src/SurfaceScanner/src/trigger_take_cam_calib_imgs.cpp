#include <rclcpp/rclcpp.hpp>
#include <std_srvs/srv/trigger.hpp>

#include <chrono>
#include <cstdlib>
#include <memory>


int main(int argc, char **argv){
  rclcpp::init(argc, argv);
  std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("trigger_cam_calib_img_node");
  rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr client = node->create_client<std_srvs::srv::Trigger>("send_cam_calib_imgs");
  auto request = std::make_shared<std_srvs::srv::Trigger::Request>();
  auto logger = node->get_logger();

  // Wait for service to be available, timeout=2s
  while (!client->wait_for_service(std::chrono::seconds(2))) {
    if (!rclcpp::ok()) {
      RCLCPP_ERROR(logger, "Interrupted while waiting for the service. Exiting.");
      return 0;
    }
    RCLCPP_INFO(logger, "camera's service not available, waiting again...");
  }
  // services found 
  auto result = client->async_send_request(request);
  // Wait for the result.
  if (rclcpp::spin_until_future_complete(node, result) ==
    rclcpp::FutureReturnCode::SUCCESS)
  {
    auto response = result.get();
    RCLCPP_INFO(logger, "Calibrated laser status: %s", response->success ? "true" : "false");
    RCLCPP_INFO(logger, "Received message:\n||%s||", response->message.c_str());
  }
  else{
    RCLCPP_WARN(logger, "Service call failed");
  }
  rclcpp::shutdown();
  return 0;
}
