// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from interfaces:msg/CameraCalibrationImgs.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "interfaces/msg/detail/camera_calibration_imgs__rosidl_typesupport_introspection_c.h"
#include "interfaces/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "interfaces/msg/detail/camera_calibration_imgs__functions.h"
#include "interfaces/msg/detail/camera_calibration_imgs__struct.h"


// Include directives for member types
// Member `imgs`
#include "sensor_msgs/msg/image.h"
// Member `imgs`
#include "sensor_msgs/msg/detail/image__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__CameraCalibrationImgs_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  interfaces__msg__CameraCalibrationImgs__init(message_memory);
}

void interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__CameraCalibrationImgs_fini_function(void * message_memory)
{
  interfaces__msg__CameraCalibrationImgs__fini(message_memory);
}

size_t interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__size_function__CameraCalibrationImgs__imgs(
  const void * untyped_member)
{
  const sensor_msgs__msg__Image__Sequence * member =
    (const sensor_msgs__msg__Image__Sequence *)(untyped_member);
  return member->size;
}

const void * interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__get_const_function__CameraCalibrationImgs__imgs(
  const void * untyped_member, size_t index)
{
  const sensor_msgs__msg__Image__Sequence * member =
    (const sensor_msgs__msg__Image__Sequence *)(untyped_member);
  return &member->data[index];
}

void * interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__get_function__CameraCalibrationImgs__imgs(
  void * untyped_member, size_t index)
{
  sensor_msgs__msg__Image__Sequence * member =
    (sensor_msgs__msg__Image__Sequence *)(untyped_member);
  return &member->data[index];
}

void interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__fetch_function__CameraCalibrationImgs__imgs(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const sensor_msgs__msg__Image * item =
    ((const sensor_msgs__msg__Image *)
    interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__get_const_function__CameraCalibrationImgs__imgs(untyped_member, index));
  sensor_msgs__msg__Image * value =
    (sensor_msgs__msg__Image *)(untyped_value);
  *value = *item;
}

void interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__assign_function__CameraCalibrationImgs__imgs(
  void * untyped_member, size_t index, const void * untyped_value)
{
  sensor_msgs__msg__Image * item =
    ((sensor_msgs__msg__Image *)
    interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__get_function__CameraCalibrationImgs__imgs(untyped_member, index));
  const sensor_msgs__msg__Image * value =
    (const sensor_msgs__msg__Image *)(untyped_value);
  *item = *value;
}

bool interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__resize_function__CameraCalibrationImgs__imgs(
  void * untyped_member, size_t size)
{
  sensor_msgs__msg__Image__Sequence * member =
    (sensor_msgs__msg__Image__Sequence *)(untyped_member);
  sensor_msgs__msg__Image__Sequence__fini(member);
  return sensor_msgs__msg__Image__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__CameraCalibrationImgs_message_member_array[1] = {
  {
    "imgs",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(interfaces__msg__CameraCalibrationImgs, imgs),  // bytes offset in struct
    NULL,  // default value
    interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__size_function__CameraCalibrationImgs__imgs,  // size() function pointer
    interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__get_const_function__CameraCalibrationImgs__imgs,  // get_const(index) function pointer
    interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__get_function__CameraCalibrationImgs__imgs,  // get(index) function pointer
    interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__fetch_function__CameraCalibrationImgs__imgs,  // fetch(index, &value) function pointer
    interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__assign_function__CameraCalibrationImgs__imgs,  // assign(index, value) function pointer
    interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__resize_function__CameraCalibrationImgs__imgs  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__CameraCalibrationImgs_message_members = {
  "interfaces__msg",  // message namespace
  "CameraCalibrationImgs",  // message name
  1,  // number of fields
  sizeof(interfaces__msg__CameraCalibrationImgs),
  interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__CameraCalibrationImgs_message_member_array,  // message members
  interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__CameraCalibrationImgs_init_function,  // function to initialize message memory (memory has to be allocated)
  interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__CameraCalibrationImgs_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__CameraCalibrationImgs_message_type_support_handle = {
  0,
  &interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__CameraCalibrationImgs_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_interfaces
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, interfaces, msg, CameraCalibrationImgs)() {
  interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__CameraCalibrationImgs_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, sensor_msgs, msg, Image)();
  if (!interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__CameraCalibrationImgs_message_type_support_handle.typesupport_identifier) {
    interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__CameraCalibrationImgs_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &interfaces__msg__CameraCalibrationImgs__rosidl_typesupport_introspection_c__CameraCalibrationImgs_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
