// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: example.proto

#ifndef PROTOBUF_example_2eproto__INCLUDED
#define PROTOBUF_example_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3000000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3000000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace example {

// Internal implementation detail -- do not call these.
void protobuf_AddDesc_example_2eproto();
void protobuf_AssignDesc_example_2eproto();
void protobuf_ShutdownFile_example_2eproto();

class User;

// ===================================================================

class User : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:example.User) */ {
 public:
  User();
  virtual ~User();

  User(const User& from);

  inline User& operator=(const User& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const User& default_instance();

  void Swap(User* other);

  // implements Message ----------------------------------------------

  inline User* New() const { return New(NULL); }

  User* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const User& from);
  void MergeFrom(const User& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const {
    return InternalSerializeWithCachedSizesToArray(false, output);
  }
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(User* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional int32 id = 1;
  void clear_id();
  static const int kIdFieldNumber = 1;
  ::google::protobuf::int32 id() const;
  void set_id(::google::protobuf::int32 value);

  // optional string name = 2;
  void clear_name();
  static const int kNameFieldNumber = 2;
  const ::std::string& name() const;
  void set_name(const ::std::string& value);
  void set_name(const char* value);
  void set_name(const char* value, size_t size);
  ::std::string* mutable_name();
  ::std::string* release_name();
  void set_allocated_name(::std::string* name);

  // repeated string emails = 3;
  int emails_size() const;
  void clear_emails();
  static const int kEmailsFieldNumber = 3;
  const ::std::string& emails(int index) const;
  ::std::string* mutable_emails(int index);
  void set_emails(int index, const ::std::string& value);
  void set_emails(int index, const char* value);
  void set_emails(int index, const char* value, size_t size);
  ::std::string* add_emails();
  void add_emails(const ::std::string& value);
  void add_emails(const char* value);
  void add_emails(const char* value, size_t size);
  const ::google::protobuf::RepeatedPtrField< ::std::string>& emails() const;
  ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_emails();

  // @@protoc_insertion_point(class_scope:example.User)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  ::google::protobuf::internal::ArenaStringPtr name_;
  ::google::protobuf::RepeatedPtrField< ::std::string> emails_;
  ::google::protobuf::int32 id_;
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_example_2eproto();
  friend void protobuf_AssignDesc_example_2eproto();
  friend void protobuf_ShutdownFile_example_2eproto();

  void InitAsDefaultInstance();
  static User* default_instance_;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// User

// optional int32 id = 1;
inline void User::clear_id() {
  id_ = 0;
}
inline ::google::protobuf::int32 User::id() const {
  // @@protoc_insertion_point(field_get:example.User.id)
  return id_;
}
inline void User::set_id(::google::protobuf::int32 value) {
  
  id_ = value;
  // @@protoc_insertion_point(field_set:example.User.id)
}

// optional string name = 2;
inline void User::clear_name() {
  name_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& User::name() const {
  // @@protoc_insertion_point(field_get:example.User.name)
  return name_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void User::set_name(const ::std::string& value) {
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:example.User.name)
}
inline void User::set_name(const char* value) {
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:example.User.name)
}
inline void User::set_name(const char* value, size_t size) {
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:example.User.name)
}
inline ::std::string* User::mutable_name() {
  
  // @@protoc_insertion_point(field_mutable:example.User.name)
  return name_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* User::release_name() {
  // @@protoc_insertion_point(field_release:example.User.name)
  
  return name_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void User::set_allocated_name(::std::string* name) {
  if (name != NULL) {
    
  } else {
    
  }
  name_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), name);
  // @@protoc_insertion_point(field_set_allocated:example.User.name)
}

// repeated string emails = 3;
inline int User::emails_size() const {
  return emails_.size();
}
inline void User::clear_emails() {
  emails_.Clear();
}
inline const ::std::string& User::emails(int index) const {
  // @@protoc_insertion_point(field_get:example.User.emails)
  return emails_.Get(index);
}
inline ::std::string* User::mutable_emails(int index) {
  // @@protoc_insertion_point(field_mutable:example.User.emails)
  return emails_.Mutable(index);
}
inline void User::set_emails(int index, const ::std::string& value) {
  // @@protoc_insertion_point(field_set:example.User.emails)
  emails_.Mutable(index)->assign(value);
}
inline void User::set_emails(int index, const char* value) {
  emails_.Mutable(index)->assign(value);
  // @@protoc_insertion_point(field_set_char:example.User.emails)
}
inline void User::set_emails(int index, const char* value, size_t size) {
  emails_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:example.User.emails)
}
inline ::std::string* User::add_emails() {
  // @@protoc_insertion_point(field_add_mutable:example.User.emails)
  return emails_.Add();
}
inline void User::add_emails(const ::std::string& value) {
  emails_.Add()->assign(value);
  // @@protoc_insertion_point(field_add:example.User.emails)
}
inline void User::add_emails(const char* value) {
  emails_.Add()->assign(value);
  // @@protoc_insertion_point(field_add_char:example.User.emails)
}
inline void User::add_emails(const char* value, size_t size) {
  emails_.Add()->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_add_pointer:example.User.emails)
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
User::emails() const {
  // @@protoc_insertion_point(field_list:example.User.emails)
  return emails_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
User::mutable_emails() {
  // @@protoc_insertion_point(field_mutable_list:example.User.emails)
  return &emails_;
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

}  // namespace example

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_example_2eproto__INCLUDED
