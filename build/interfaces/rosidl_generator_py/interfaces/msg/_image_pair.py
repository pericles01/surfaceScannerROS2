# generated from rosidl_generator_py/resource/_idl.py.em
# with input from interfaces:msg/ImagePair.idl
# generated code does not contain a copyright notice


# Import statements for member types

import builtins  # noqa: E402, I100

import rosidl_parser.definition  # noqa: E402, I100


class Metaclass_ImagePair(type):
    """Metaclass of message 'ImagePair'."""

    _CREATE_ROS_MESSAGE = None
    _CONVERT_FROM_PY = None
    _CONVERT_TO_PY = None
    _DESTROY_ROS_MESSAGE = None
    _TYPE_SUPPORT = None

    __constants = {
    }

    @classmethod
    def __import_type_support__(cls):
        try:
            from rosidl_generator_py import import_type_support
            module = import_type_support('interfaces')
        except ImportError:
            import logging
            import traceback
            logger = logging.getLogger(
                'interfaces.msg.ImagePair')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__msg__image_pair
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__msg__image_pair
            cls._CONVERT_TO_PY = module.convert_to_py_msg__msg__image_pair
            cls._TYPE_SUPPORT = module.type_support_msg__msg__image_pair
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__msg__image_pair

            from sensor_msgs.msg import Image
            if Image.__class__._TYPE_SUPPORT is None:
                Image.__class__.__import_type_support__()

    @classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
        }


class ImagePair(metaclass=Metaclass_ImagePair):
    """Message class 'ImagePair'."""

    __slots__ = [
        '_is_for_laser_calib',
        '_origin_img',
        '_laser_img',
    ]

    _fields_and_field_types = {
        'is_for_laser_calib': 'boolean',
        'origin_img': 'sensor_msgs/Image',
        'laser_img': 'sensor_msgs/Image',
    }

    SLOT_TYPES = (
        rosidl_parser.definition.BasicType('boolean'),  # noqa: E501
        rosidl_parser.definition.NamespacedType(['sensor_msgs', 'msg'], 'Image'),  # noqa: E501
        rosidl_parser.definition.NamespacedType(['sensor_msgs', 'msg'], 'Image'),  # noqa: E501
    )

    def __init__(self, **kwargs):
        assert all('_' + key in self.__slots__ for key in kwargs.keys()), \
            'Invalid arguments passed to constructor: %s' % \
            ', '.join(sorted(k for k in kwargs.keys() if '_' + k not in self.__slots__))
        self.is_for_laser_calib = kwargs.get('is_for_laser_calib', bool())
        from sensor_msgs.msg import Image
        self.origin_img = kwargs.get('origin_img', Image())
        from sensor_msgs.msg import Image
        self.laser_img = kwargs.get('laser_img', Image())

    def __repr__(self):
        typename = self.__class__.__module__.split('.')
        typename.pop()
        typename.append(self.__class__.__name__)
        args = []
        for s, t in zip(self.__slots__, self.SLOT_TYPES):
            field = getattr(self, s)
            fieldstr = repr(field)
            # We use Python array type for fields that can be directly stored
            # in them, and "normal" sequences for everything else.  If it is
            # a type that we store in an array, strip off the 'array' portion.
            if (
                isinstance(t, rosidl_parser.definition.AbstractSequence) and
                isinstance(t.value_type, rosidl_parser.definition.BasicType) and
                t.value_type.typename in ['float', 'double', 'int8', 'uint8', 'int16', 'uint16', 'int32', 'uint32', 'int64', 'uint64']
            ):
                if len(field) == 0:
                    fieldstr = '[]'
                else:
                    assert fieldstr.startswith('array(')
                    prefix = "array('X', "
                    suffix = ')'
                    fieldstr = fieldstr[len(prefix):-len(suffix)]
            args.append(s[1:] + '=' + fieldstr)
        return '%s(%s)' % ('.'.join(typename), ', '.join(args))

    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            return False
        if self.is_for_laser_calib != other.is_for_laser_calib:
            return False
        if self.origin_img != other.origin_img:
            return False
        if self.laser_img != other.laser_img:
            return False
        return True

    @classmethod
    def get_fields_and_field_types(cls):
        from copy import copy
        return copy(cls._fields_and_field_types)

    @builtins.property
    def is_for_laser_calib(self):
        """Message field 'is_for_laser_calib'."""
        return self._is_for_laser_calib

    @is_for_laser_calib.setter
    def is_for_laser_calib(self, value):
        if __debug__:
            assert \
                isinstance(value, bool), \
                "The 'is_for_laser_calib' field must be of type 'bool'"
        self._is_for_laser_calib = value

    @builtins.property
    def origin_img(self):
        """Message field 'origin_img'."""
        return self._origin_img

    @origin_img.setter
    def origin_img(self, value):
        if __debug__:
            from sensor_msgs.msg import Image
            assert \
                isinstance(value, Image), \
                "The 'origin_img' field must be a sub message of type 'Image'"
        self._origin_img = value

    @builtins.property
    def laser_img(self):
        """Message field 'laser_img'."""
        return self._laser_img

    @laser_img.setter
    def laser_img(self, value):
        if __debug__:
            from sensor_msgs.msg import Image
            assert \
                isinstance(value, Image), \
                "The 'laser_img' field must be a sub message of type 'Image'"
        self._laser_img = value
