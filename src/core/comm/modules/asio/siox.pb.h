// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: siox.proto

#ifndef PROTOBUF_siox_2eproto__INCLUDED
#define PROTOBUF_siox_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2005000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace buffers {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_siox_2eproto();
void protobuf_AssignDesc_siox_2eproto();
void protobuf_ShutdownFile_siox_2eproto();

class MessageBuffer;

enum MessageBuffer_Action {
  MessageBuffer_Action_Advertise = 0,
  MessageBuffer_Action_Subscribe = 1,
  MessageBuffer_Action_Message = 2
};
bool MessageBuffer_Action_IsValid(int value);
const MessageBuffer_Action MessageBuffer_Action_Action_MIN = MessageBuffer_Action_Advertise;
const MessageBuffer_Action MessageBuffer_Action_Action_MAX = MessageBuffer_Action_Message;
const int MessageBuffer_Action_Action_ARRAYSIZE = MessageBuffer_Action_Action_MAX + 1;

const ::google::protobuf::EnumDescriptor* MessageBuffer_Action_descriptor();
inline const ::std::string& MessageBuffer_Action_Name(MessageBuffer_Action value) {
  return ::google::protobuf::internal::NameOfEnum(
    MessageBuffer_Action_descriptor(), value);
}
inline bool MessageBuffer_Action_Parse(
    const ::std::string& name, MessageBuffer_Action* value) {
  return ::google::protobuf::internal::ParseNamedEnum<MessageBuffer_Action>(
    MessageBuffer_Action_descriptor(), name, value);
}
// ===================================================================

class MessageBuffer : public ::google::protobuf::Message {
 public:
  MessageBuffer();
  virtual ~MessageBuffer();

  MessageBuffer(const MessageBuffer& from);

  inline MessageBuffer& operator=(const MessageBuffer& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const MessageBuffer& default_instance();

  void Swap(MessageBuffer* other);

  // implements Message ----------------------------------------------

  MessageBuffer* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const MessageBuffer& from);
  void MergeFrom(const MessageBuffer& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  typedef MessageBuffer_Action Action;
  static const Action Advertise = MessageBuffer_Action_Advertise;
  static const Action Subscribe = MessageBuffer_Action_Subscribe;
  static const Action Message = MessageBuffer_Action_Message;
  static inline bool Action_IsValid(int value) {
    return MessageBuffer_Action_IsValid(value);
  }
  static const Action Action_MIN =
    MessageBuffer_Action_Action_MIN;
  static const Action Action_MAX =
    MessageBuffer_Action_Action_MAX;
  static const int Action_ARRAYSIZE =
    MessageBuffer_Action_Action_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  Action_descriptor() {
    return MessageBuffer_Action_descriptor();
  }
  static inline const ::std::string& Action_Name(Action value) {
    return MessageBuffer_Action_Name(value);
  }
  static inline bool Action_Parse(const ::std::string& name,
      Action* value) {
    return MessageBuffer_Action_Parse(name, value);
  }

  // accessors -------------------------------------------------------

  // required .buffers.MessageBuffer.Action action = 1 [default = Message];
  inline bool has_action() const;
  inline void clear_action();
  static const int kActionFieldNumber = 1;
  inline ::buffers::MessageBuffer_Action action() const;
  inline void set_action(::buffers::MessageBuffer_Action value);

  // required uint32 type = 2;
  inline bool has_type() const;
  inline void clear_type();
  static const int kTypeFieldNumber = 2;
  inline ::google::protobuf::uint32 type() const;
  inline void set_type(::google::protobuf::uint32 value);

  // optional uint32 priority = 3;
  inline bool has_priority() const;
  inline void clear_priority();
  static const int kPriorityFieldNumber = 3;
  inline ::google::protobuf::uint32 priority() const;
  inline void set_priority(::google::protobuf::uint32 value);

  // optional uint64 msg_size = 4;
  inline bool has_msg_size() const;
  inline void clear_msg_size();
  static const int kMsgSizeFieldNumber = 4;
  inline ::google::protobuf::uint64 msg_size() const;
  inline void set_msg_size(::google::protobuf::uint64 value);

  // optional bytes msg_data = 5;
  inline bool has_msg_data() const;
  inline void clear_msg_data();
  static const int kMsgDataFieldNumber = 5;
  inline const ::std::string& msg_data() const;
  inline void set_msg_data(const ::std::string& value);
  inline void set_msg_data(const char* value);
  inline void set_msg_data(const void* value, size_t size);
  inline ::std::string* mutable_msg_data();
  inline ::std::string* release_msg_data();
  inline void set_allocated_msg_data(::std::string* msg_data);

  // @@protoc_insertion_point(class_scope:buffers.MessageBuffer)
 private:
  inline void set_has_action();
  inline void clear_has_action();
  inline void set_has_type();
  inline void clear_has_type();
  inline void set_has_priority();
  inline void clear_has_priority();
  inline void set_has_msg_size();
  inline void clear_has_msg_size();
  inline void set_has_msg_data();
  inline void clear_has_msg_data();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  int action_;
  ::google::protobuf::uint32 type_;
  ::google::protobuf::uint64 msg_size_;
  ::std::string* msg_data_;
  ::google::protobuf::uint32 priority_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(5 + 31) / 32];

  friend void  protobuf_AddDesc_siox_2eproto();
  friend void protobuf_AssignDesc_siox_2eproto();
  friend void protobuf_ShutdownFile_siox_2eproto();

  void InitAsDefaultInstance();
  static MessageBuffer* default_instance_;
};
// ===================================================================


// ===================================================================

// MessageBuffer

// required .buffers.MessageBuffer.Action action = 1 [default = Message];
inline bool MessageBuffer::has_action() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void MessageBuffer::set_has_action() {
  _has_bits_[0] |= 0x00000001u;
}
inline void MessageBuffer::clear_has_action() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void MessageBuffer::clear_action() {
  action_ = 2;
  clear_has_action();
}
inline ::buffers::MessageBuffer_Action MessageBuffer::action() const {
  return static_cast< ::buffers::MessageBuffer_Action >(action_);
}
inline void MessageBuffer::set_action(::buffers::MessageBuffer_Action value) {
  assert(::buffers::MessageBuffer_Action_IsValid(value));
  set_has_action();
  action_ = value;
}

// required uint32 type = 2;
inline bool MessageBuffer::has_type() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void MessageBuffer::set_has_type() {
  _has_bits_[0] |= 0x00000002u;
}
inline void MessageBuffer::clear_has_type() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void MessageBuffer::clear_type() {
  type_ = 0u;
  clear_has_type();
}
inline ::google::protobuf::uint32 MessageBuffer::type() const {
  return type_;
}
inline void MessageBuffer::set_type(::google::protobuf::uint32 value) {
  set_has_type();
  type_ = value;
}

// optional uint32 priority = 3;
inline bool MessageBuffer::has_priority() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void MessageBuffer::set_has_priority() {
  _has_bits_[0] |= 0x00000004u;
}
inline void MessageBuffer::clear_has_priority() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void MessageBuffer::clear_priority() {
  priority_ = 0u;
  clear_has_priority();
}
inline ::google::protobuf::uint32 MessageBuffer::priority() const {
  return priority_;
}
inline void MessageBuffer::set_priority(::google::protobuf::uint32 value) {
  set_has_priority();
  priority_ = value;
}

// optional uint64 msg_size = 4;
inline bool MessageBuffer::has_msg_size() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void MessageBuffer::set_has_msg_size() {
  _has_bits_[0] |= 0x00000008u;
}
inline void MessageBuffer::clear_has_msg_size() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void MessageBuffer::clear_msg_size() {
  msg_size_ = GOOGLE_ULONGLONG(0);
  clear_has_msg_size();
}
inline ::google::protobuf::uint64 MessageBuffer::msg_size() const {
  return msg_size_;
}
inline void MessageBuffer::set_msg_size(::google::protobuf::uint64 value) {
  set_has_msg_size();
  msg_size_ = value;
}

// optional bytes msg_data = 5;
inline bool MessageBuffer::has_msg_data() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void MessageBuffer::set_has_msg_data() {
  _has_bits_[0] |= 0x00000010u;
}
inline void MessageBuffer::clear_has_msg_data() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void MessageBuffer::clear_msg_data() {
  if (msg_data_ != &::google::protobuf::internal::kEmptyString) {
    msg_data_->clear();
  }
  clear_has_msg_data();
}
inline const ::std::string& MessageBuffer::msg_data() const {
  return *msg_data_;
}
inline void MessageBuffer::set_msg_data(const ::std::string& value) {
  set_has_msg_data();
  if (msg_data_ == &::google::protobuf::internal::kEmptyString) {
    msg_data_ = new ::std::string;
  }
  msg_data_->assign(value);
}
inline void MessageBuffer::set_msg_data(const char* value) {
  set_has_msg_data();
  if (msg_data_ == &::google::protobuf::internal::kEmptyString) {
    msg_data_ = new ::std::string;
  }
  msg_data_->assign(value);
}
inline void MessageBuffer::set_msg_data(const void* value, size_t size) {
  set_has_msg_data();
  if (msg_data_ == &::google::protobuf::internal::kEmptyString) {
    msg_data_ = new ::std::string;
  }
  msg_data_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* MessageBuffer::mutable_msg_data() {
  set_has_msg_data();
  if (msg_data_ == &::google::protobuf::internal::kEmptyString) {
    msg_data_ = new ::std::string;
  }
  return msg_data_;
}
inline ::std::string* MessageBuffer::release_msg_data() {
  clear_has_msg_data();
  if (msg_data_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = msg_data_;
    msg_data_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void MessageBuffer::set_allocated_msg_data(::std::string* msg_data) {
  if (msg_data_ != &::google::protobuf::internal::kEmptyString) {
    delete msg_data_;
  }
  if (msg_data) {
    set_has_msg_data();
    msg_data_ = msg_data;
  } else {
    clear_has_msg_data();
    msg_data_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace buffers

#ifndef SWIG
namespace google {
namespace protobuf {

template <>
inline const EnumDescriptor* GetEnumDescriptor< ::buffers::MessageBuffer_Action>() {
  return ::buffers::MessageBuffer_Action_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_siox_2eproto__INCLUDED
