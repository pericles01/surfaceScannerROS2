// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from interfaces:msg/ImagePair.idl
// generated code does not contain a copyright notice

#ifndef INTERFACES__MSG__DETAIL__IMAGE_PAIR__TRAITS_HPP_
#define INTERFACES__MSG__DETAIL__IMAGE_PAIR__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "interfaces/msg/detail/image_pair__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'origin_img'
// Member 'laser_img'
#include "sensor_msgs/msg/detail/image__traits.hpp"

namespace interfaces
{

namespace msg
{

inline void to_flow_style_yaml(
  const ImagePair & msg,
  std::ostream & out)
{
  out << "{";
  // member: is_for_laser_calib
  {
    out << "is_for_laser_calib: ";
    rosidl_generator_traits::value_to_yaml(msg.is_for_laser_calib, out);
    out << ", ";
  }

  // member: origin_img
  {
    out << "origin_img: ";
    to_flow_style_yaml(msg.origin_img, out);
    out << ", ";
  }

  // member: laser_img
  {
    out << "laser_img: ";
    to_flow_style_yaml(msg.laser_img, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const ImagePair & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: is_for_laser_calib
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "is_for_laser_calib: ";
    rosidl_generator_traits::value_to_yaml(msg.is_for_laser_calib, out);
    out << "\n";
  }

  // member: origin_img
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "origin_img:\n";
    to_block_style_yaml(msg.origin_img, out, indentation + 2);
  }

  // member: laser_img
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "laser_img:\n";
    to_block_style_yaml(msg.laser_img, out, indentation + 2);
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const ImagePair & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace msg

}  // namespace interfaces

namespace rosidl_generator_traits
{

[[deprecated("use interfaces::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const interfaces::msg::ImagePair & msg,
  std::ostream & out, size_t indentation = 0)
{
  interfaces::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use interfaces::msg::to_yaml() instead")]]
inline std::string to_yaml(const interfaces::msg::ImagePair & msg)
{
  return interfaces::msg::to_yaml(msg);
}

template<>
inline const char * data_type<interfaces::msg::ImagePair>()
{
  return "interfaces::msg::ImagePair";
}

template<>
inline const char * name<interfaces::msg::ImagePair>()
{
  return "interfaces/msg/ImagePair";
}

template<>
struct has_fixed_size<interfaces::msg::ImagePair>
  : std::integral_constant<bool, has_fixed_size<sensor_msgs::msg::Image>::value> {};

template<>
struct has_bounded_size<interfaces::msg::ImagePair>
  : std::integral_constant<bool, has_bounded_size<sensor_msgs::msg::Image>::value> {};

template<>
struct is_message<interfaces::msg::ImagePair>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // INTERFACES__MSG__DETAIL__IMAGE_PAIR__TRAITS_HPP_
