
#ifndef HOOKMANAGER_ART_METHOD_H
#define HOOKMANAGER_ART_METHOD_H

#include "globals.h"
#include "../sys/cdefs.h"
#include "../base/macros.h"

#define MANAGED PACKED(4)



class PACKED(4) Object {
public:
    uint32_t klass_;
    uint32_t monitor_;
};
class PACKED(4) ObjectArray:public Object{
public:
    int32_t size;
    u4 elements[];
};
class MANAGED PointerArray:public Object{
public:
    int32_t size;
#ifdef __LP64__
    u4 stub;
    u8 elements[];
#else
    u4 elements[];
#endif
};
namespace art{
    template<typename T>
    static constexpr T RoundUp(T x, typename std::remove_reference<T>::type n) {
        return (x + n - 1)& -n;
    }
    template<typename T>
    class Array {
    public:
        explicit Array(size_t length)
                : size_(static_cast<uint32_t>(length)) {}

        T& At(size_t index, size_t element_size = sizeof(T), size_t alignment = alignof(T)) {
            return AtUnchecked(index, element_size, alignment);
        }

        const T& At(size_t index, size_t element_size = sizeof(T), size_t alignment = alignof(T)) const {

            return AtUnchecked(index, element_size, alignment);
        }


        static size_t OffsetOfElement(size_t index,
                                      size_t element_size = sizeof(T),
                                      size_t alignment = alignof(T)) {
            return RoundUp(offsetof(Array<T>, data), alignment) + index * element_size;
        }

        static size_t ComputeSize(size_t num_elements,
                                  size_t element_size = sizeof(T),
                                  size_t alignment = alignof(T)) {
            size_t result = OffsetOfElement(num_elements, element_size, alignment);
            return result;
        }

        size_t size() const {
            return size_;
        }

        // Update the length but does not reallocate storage.
        void SetSize(size_t length) {
            size_ = static_cast<uint32_t>(length);
        }

        // Clear the potentially uninitialized padding between the size_ and actual data.
        void ClearPadding(size_t element_size = sizeof(T), size_t alignment = alignof(T)) {
            size_t gap_offset = offsetof(Array<T>, data);
            size_t gap_size = OffsetOfElement(0, element_size, alignment) - gap_offset;
            memset(reinterpret_cast<uint8_t*>(this) + gap_offset, 0, gap_size);
        }

    private:
        T& AtUnchecked(size_t index, size_t element_size, size_t alignment) {
            return *reinterpret_cast<T*>(
                    reinterpret_cast<uintptr_t>(this) + OffsetOfElement(index, element_size, alignment));
        }

        const T& AtUnchecked(size_t index, size_t element_size, size_t alignment) const {
            return *reinterpret_cast<T*>(
                    reinterpret_cast<uintptr_t>(this) + OffsetOfElement(index, element_size, alignment));
        }

        uint32_t size_;
        uint8_t data[0];
    };

};


class MANAGED NougatDexCache : public Object {
public:
    uint32_t dex_;
    uint32_t location_;
    uint64_t dex_file_;           // const DexFile*
    uint64_t resolved_fields_;    // ArtField*, array with num_resolved_fields_ elements.
    uint64_t resolved_methods_;   // ArtMethod*, array with num_resolved_methods_ elements.
    uint64_t resolved_types_;     // GcRoot<Class>*, array with num_resolved_types_ elements.
    uint64_t strings_;
};

class MANAGED KitkatDexCache : public Object {
public:
    Object *dex_;
    void *initialized_static_storage_;
    void *location_;
    void *resolved_fields_;
    void *resolved_methods_;
    void *resolved_types_;
    void *strings_;
    uint32_t dex_file_;
};

class MANAGED DexCache : public Object {
public:
    uint32_t dex_;
    uint32_t location_;
    uint32_t resolved_fields_;
    uint32_t resolved_methods_;
    uint32_t resolved_types_;
    uint32_t strings_;
    uint64_t dex_file_;
};

class NougatClass;
class ArtClass;
class MANAGED ArtClass : public Object {
public:
    // 'Class' Object Fields
    // Order governed by java field ordering. See art::ClassLinker::LinkFields.
    //since Nougat
    //uint32_t annotation_type_;

    // defining class loader, or NULL for the "bootstrap" system loader
    uint32_t class_loader_;
    // For array classes, the component class object for instanceof/checkcast
    // (for String[][][], this will be String[][]). NULL for non-array classes.
    uint32_t component_type_;
    // DexCache of resolved constant pool entries (will be NULL for classes generated by the
    // runtime such as arrays and primitive classes).
    uint32_t dex_cache_;

    u4 getDexCache(bool isNougat) ;
};
class MANAGED AnnoType{
public:// Order governed by java field ordering. See art::ClassLinker::LinkFields.
    uint32_t annotation_type_;
};

class MANAGED NougatClass: public Object {
    enum Status {
        kStatusRetired = -2, // Retired, should not be used. Use the newly cloned one instead.
        kStatusError = -1,
        kStatusNotReady = 0,
        kStatusIdx = 1, // Loaded, DEX idx in super_class_type_idx_ and interfaces_type_idx_.
        kStatusLoaded = 2,  // DEX idx values resolved.
        kStatusResolving = 3,  // Just cloned from temporary class object.
        kStatusResolved = 4,  // Part of linking.
        kStatusVerifying = 5,  // In the process of being verified.
        kStatusRetryVerificationAtRuntime = 6, // Compile time verification failed, retry at runtime.
        kStatusVerifyingAtRuntime = 7,  // Retrying verification at runtime.
        kStatusVerified = 8,  // Logically part of linking; done pre-init.
        kStatusInitializing = 9,  // Class init in progress.
        kStatusInitialized = 10,  // Ready to go.
        kStatusMax = 11,
    };
public:
    uint32_t annotation_type_;

    // defining class loader, or NULL for the "bootstrap" system loader
    uint32_t class_loader_;
    // For array classes, the component class object for instanceof/checkcast
    // (for String[][][], this will be String[][]). NULL for non-array classes.
    uint32_t component_type_;
    // DexCache of resolved constant pool entries (will be NULL for classes generated by the
    // runtime such as arrays and primitive classes).
    uint32_t dex_cache_;
    // The interface table (iftable_) contains pairs of a interface class and an array of the
    // interface methods. There is one pair per interface supported by this class.  That means one
    // pair for each interface we support directly, indirectly via superclass, or indirectly via a
    // superinterface.  This will be null if neither we nor our superclass implement any interfaces.
    //
    // Why we need this: given "class Foo implements Face", declare "Face faceObj = new Foo()".
    // Invoke faceObj.blah(), where "blah" is part of the Face interface.  We can't easily use a
    // single vtable.
    //
    // For every interface a concrete class implements, we create an array of the concrete vtable_
    // methods for the methods in the interface.
    uint32_t iftable_;
    // Descriptor for the class such as "java.lang.Class" or "[C". Lazily initialized by ComputeName
    uint32_t name_;
    // The superclass, or null if this is java.lang.Object or a primitive type.
    //
    // Note that interfaces have java.lang.Object as their
    // superclass. This doesn't match the expectations in JNI
    // GetSuperClass or java.lang.Class.getSuperClass() which need to
    // check for interfaces and return null.
    uint32_t super_class_;

    // If class verify fails, we must return same error on subsequent tries. We may store either
    // the class of the error, or an actual instance of Throwable here.
    uint32_t verify_error_;
    // Virtual method table (vtable), for use by "invoke-virtual".  The vtable from the superclass is
    // copied in, and virtual methods from our class either replace those from the super or are
    // appended. For abstract classes, methods may be created in the vtable that aren't in
    // virtual_ methods_ for miranda methods.
    uint32_t vtable_;

    // Access flags; low 16 bits are defined by VM spec.
    // Note: Shuffled back.
    uint32_t access_flags_;
    // Short cuts to dex_cache_ member for fast compiled code access.
    uint64_t dex_cache_strings_;

    // instance fields
    //
    // These describe the layout of the contents of an Object.
    // Note that only the fields directly declared by this class are
    // listed in ifields; fields declared by a superclass are listed in
    // the superclass's Class.ifields.
    //
    // ArtFields are allocated as a length prefixed ArtField array, and not an array of pointers to
    // ArtFields.
    uint64_t ifields_;
    // Pointer to an ArtMethod length-prefixed array. All the methods where this class is the place
    // where they are logically defined. This includes all private, static, final and virtual methods
    // as well as inherited default methods and miranda methods.
    //
    // The slice methods_ [0, virtual_methods_offset_) are the direct (static, private, init) methods
    // declared by this class.
    //
    // The slice methods_ [virtual_methods_offset_, copied_methods_offset_) are the virtual methods
    // declared by this class.
    //
    // The slice methods_ [copied_methods_offset_, |methods_|) are the methods that are copied from
    // interfaces such as miranda or default methods. These are copied for resolution purposes as this
    // class is where they are (logically) declared as far as the virtual dispatch is concerned.
    //
    // Note that this field is used by the native debugger as the unique identifier for the type.
    uint64_t methods_;

    // Static fields length-prefixed array.
    uint64_t sfields_;

    // Class flags to help speed up visiting object references.
    uint32_t class_flags_;

    // Total size of the Class instance; used when allocating storage on gc heap.
    // See also object_size_.
    uint32_t class_size_;
    // Tid used to check for recursive <clinit> invocation.
    pid_t clinit_thread_id_;
    // ClassDef index in dex file, -1 if no class definition such as an array.
    // TODO: really 16bits
    int32_t dex_class_def_idx_;
    // Type index in dex file.
    // TODO: really 16bits
    int32_t dex_type_idx_;
    // Number of instance fields that are object refs.
    uint32_t num_reference_instance_fields_;
    // Number of static fields that are object refs,
    uint32_t num_reference_static_fields_;
    // Total object size; used when allocating storage on gc heap.
    // (For interfaces and abstract classes this will be zero.)
    // See also class_size_.
    uint32_t object_size_;
    // The lower 16 bits contains a Primitive::Type value. The upper 16
    // bits contains the size shift of the primitive type.
    uint32_t primitive_type_;
    // Bitmap of offsets of ifields.
    uint32_t reference_instance_offsets_;
    // State of class initialization.
    Status status_;
    // default, and default-conflict methods. Having a hard limit of ((2 << 16) - 1) for methods
    // defined on a single class is well established in Java so we will use only uint16_t's here.
    uint16_t copied_methods_offset_;

    // The offset of the first declared virtual methods in the methods_ array.
    uint16_t virtual_methods_;//aka:virtual_method_offset_
};
inline u4 ArtClass::getDexCache(bool isNougat) {
    if (isNougat){
        return reinterpret_cast<NougatClass*>(this)->dex_cache_;
    }
    else return dex_cache_;
}
class MANAGED MarClass:public ArtClass{

public:
    // Short cuts to dex_cache_ member for fast compiled code access.
    uint32_t dex_cache_strings_;
    // The interface table (iftable_) contains pairs of a interface class and an array of the
    // interface methods. There is one pair per interface supported by this class.  That means one
    // pair for each interface we support directly, indirectly via superclass, or indirectly via a
    // superinterface.  This will be null if neither we nor our superclass implement any interfaces.
    //
    // Why we need this: given "class Foo implements Face", declare "Face faceObj = new Foo()".
    // Invoke faceObj.blah(), where "blah" is part of the Face interface.  We can't easily use a
    // single vtable.
    //
    // For every interface a concrete class implements, we create an array of the concrete vtable_
    // methods for the methods in the interface.
    uint32_t iftable_;
    // Descriptor for the class such as "java.lang.Class" or "[C". Lazily initialized by ComputeName
    uint32_t name_;
    // The superclass, or null if this is java.lang.Object or a primitive type.
    //
    // Note that interfaces have java.lang.Object as their
    // superclass. This doesn't match the expectations in JNI
    // GetSuperClass or java.lang.Class.getSuperClass() which need to
    // check for interfaces and return null.
    uint32_t super_class_;
    // If class verify fails, we must return same error on subsequent tries.
    uint32_t verify_error_class_;
    // Virtual method table (vtable), for use by "invoke-virtual".  The vtable from the superclass is
    // copied in, and virtual methods from our class either replace those from the super or are
    // appended. For abstract classes, methods may be created in the vtable that aren't in
    // virtual_ methods_ for miranda methods.
    uint32_t vtable_;
    // Access flags; low 16 bits are defined by VM spec.
    // Note: Shuffled back.
    uint32_t access_flags_;
    // static, private, and <init> methods. Pointer to an ArtMethod length-prefixed array.
    uint64_t direct_methods_;
    // instance fields
    //
    // These describe the layout of the contents of an Object.
    // Note that only the fields directly declared by this class are
    // listed in ifields; fields declared by a superclass are listed in
    // the superclass's Class.ifields.
    //
    // ArtFields are allocated as a length prefixed ArtField array, and not an array of pointers to
    // ArtFields.
    uint64_t ifields_;
    // Static fields length-prefixed array.
    uint64_t sfields_;
    // Virtual methods defined in this class; invoked through vtable. Pointer to an ArtMethod
    // length-prefixed array.
    uint64_t virtual_methods_;
    // Total size of the Class instance; used when allocating storage on gc heap.
    // See also object_size_.
    uint32_t class_size_;
    // Tid used to check for recursive <clinit> invocation.
    pid_t clinit_thread_id_;
    // ClassDef index in dex file, -1 if no class definition such as an array.
    // TODO: really 16bits
    int32_t dex_class_def_idx_;
    // Type index in dex file.
    // TODO: really 16bits
    int32_t dex_type_idx_;
    // Number of direct fields.
    uint32_t num_direct_methods_;
    // Number of instance fields.
    uint32_t num_instance_fields_;
    // Number of instance fields that are object refs.
    uint32_t num_reference_instance_fields_;
    // Number of static fields that are object refs,
    uint32_t num_reference_static_fields_;
    // Number of static fields.
    uint32_t num_static_fields_;
    // Number of virtual methods.
    uint32_t num_virtual_methods_;
    // Total object size; used when allocating storage on gc heap.
    // (For interfaces and abstract classes this will be zero.)
    // See also class_size_.
    uint32_t object_size_;
    // The lower 16 bits contains a Primitive::Type value. The upper 16
    // bits contains the size shift of the primitive type.
    uint32_t primitive_type_;
    // Bitmap of offsets of ifields.
    uint32_t reference_instance_offsets_;
    // State of class initialization.
    uint32_t status_;
};

class MANAGED LMr1Class:public ArtClass{

public:
    uint32_t dex_cache_strings_;
    // static, private, and <init> methods
    uint32_t direct_methods_;
    // instance fields
    //
    // These describe the layout of the contents of an Object.
    // Note that only the fields directly declared by this class are
    // listed in ifields; fields declared by a superclass are listed in
    // the superclass's Class.ifields.
    //
    // All instance fields that refer to objects are guaranteed to be at
    // the beginning of the field list.  num_reference_instance_fields_
    // specifies the number of reference fields.
    uint32_t ifields_;
    // The interface table (iftable_) contains pairs of a interface class and an array of the
    // interface methods. There is one pair per interface supported by this class.  That means one
    // pair for each interface we support directly, indirectly via superclass, or indirectly via a
    // superinterface.  This will be null if neither we nor our superclass implement any interfaces.
    //
    // Why we need this: given "class Foo implements Face", declare "Face faceObj = new Foo()".
    // Invoke faceObj.blah(), where "blah" is part of the Face interface.  We can't easily use a
    // single vtable.
    //
    // For every interface a concrete class implements, we create an array of the concrete vtable_
    // methods for the methods in the interface.
    uint32_t iftable_;
    // Descriptor for the class such as "java.lang.Class" or "[C". Lazily initialized by ComputeName
    uint32_t name_;
    // Static fields
    uint32_t sfields_;
    // The superclass, or NULL if this is java.lang.Object, an interface or primitive type.
    uint32_t super_class_;
    // If class verify fails, we must return same error on subsequent tries.
    uint32_t verify_error_class_;
    // Virtual methods defined in this class; invoked through vtable.
    uint32_t virtual_methods_;
    // Virtual method table (vtable), for use by "invoke-virtual".  The vtable from the superclass is
    // copied in, and virtual methods from our class either replace those from the super or are
    // appended. For abstract classes, methods may be created in the vtable that aren't in
    // virtual_ methods_ for miranda methods.
    uint32_t vtable_;

    // Access flags; low 16 bits are defined by VM spec.
    uint32_t access_flags_;
    // Total size of the Class instance; used when allocating storage on gc heap.
    // See also object_size_.
    uint32_t class_size_;
    // Tid used to check for recursive <clinit> invocation.
    pid_t clinit_thread_id_;
    // ClassDef index in dex file, -1 if no class definition such as an array.
    // TODO: really 16bits
    int32_t dex_class_def_idx_;
    // Type index in dex file.
    // TODO: really 16bits
    int32_t dex_type_idx_;
    // Number of instance fields that are object refs.
    uint32_t num_reference_instance_fields_;
    // Number of static fields that are object refs,
    uint32_t num_reference_static_fields_;
    // Total object size; used when allocating storage on gc heap.
    // (For interfaces and abstract classes this will be zero.)
    // See also class_size_.
    uint32_t object_size_;
    // Primitive type value, or Primitive::kPrimNot (0); set for generated primitive classes.
    uint32_t primitive_type_;
    // Bitmap of offsets of ifields.
    uint32_t reference_instance_offsets_;
    // Bitmap of offsets of sfields.
    uint32_t reference_static_offsets_;
    // State of class initialization.
    int32_t status_;
};
class LClass:public ArtClass{
public:
    // static, private, and <init> methods
    uint32_t direct_methods_;
    // instance fields
    //
    // These describe the layout of the contents of an Object.
    // Note that only the fields directly declared by this class are
    // listed in ifields; fields declared by a superclass are listed in
    // the superclass's Class.ifields.
    //
    // All instance fields that refer to objects are guaranteed to be at
    // the beginning of the field list.  num_reference_instance_fields_
    // specifies the number of reference fields.
    uint32_t ifields_;
    // The interface table (iftable_) contains pairs of a interface class and an array of the
    // interface methods. There is one pair per interface supported by this class.  That means one
    // pair for each interface we support directly, indirectly via superclass, or indirectly via a
    // superinterface.  This will be null if neither we nor our superclass implement any interfaces.
    //
    // Why we need this: given "class Foo implements Face", declare "Face faceObj = new Foo()".
    // Invoke faceObj.blah(), where "blah" is part of the Face interface.  We can't easily use a
    // single vtable.
    //
    // For every interface a concrete class implements, we create an array of the concrete vtable_
    // methods for the methods in the interface.
    uint32_t iftable_;
    // Interface method table (imt), for quick "invoke-interface".
    uint32_t imtable_;
    // Descriptor for the class such as "java.lang.Class" or "[C". Lazily initialized by ComputeName
    uint32_t name_;
    // Static fields
    uint32_t sfields_;
    // The superclass, or NULL if this is java.lang.Object, an interface or primitive type.
    uint32_t super_class_;
    // If class verify fails, we must return same error on subsequent tries.
    uint32_t verify_error_class_;
    // Virtual methods defined in this class; invoked through vtable.
    uint32_t virtual_methods_;
    // Virtual method table (vtable), for use by "invoke-virtual".  The vtable from the superclass is
    // copied in, and virtual methods from our class either replace those from the super or are
    // appended. For abstract classes, methods may be created in the vtable that aren't in
    // virtual_ methods_ for miranda methods.
    uint32_t vtable_;

    // Access flags; low 16 bits are defined by VM spec.
    uint32_t access_flags_;
    // Total size of the Class instance; used when allocating storage on gc heap.
    // See also object_size_.
    uint32_t class_size_;
    // Tid used to check for recursive <clinit> invocation.
    pid_t clinit_thread_id_;
    // ClassDef index in dex file, -1 if no class definition such as an array.
    // TODO: really 16bits
    int32_t dex_class_def_idx_;
    // Type index in dex file.
    // TODO: really 16bits
    int32_t dex_type_idx_;
    // Number of instance fields that are object refs.
    uint32_t num_reference_instance_fields_;
    // Number of static fields that are object refs,
    uint32_t num_reference_static_fields_;
    // Total object size; used when allocating storage on gc heap.
    // (For interfaces and abstract classes this will be zero.)
    // See also class_size_.
    uint32_t object_size_;
    // Primitive type value, or Primitive::kPrimNot (0); set for generated primitive classes.
    uint32_t primitive_type_;
    // Bitmap of offsets of ifields.
    uint32_t reference_instance_offsets_;
    // Bitmap of offsets of sfields.
    uint32_t reference_static_offsets_;
    // State of class initialization.
    int32_t status_;
    // TODO: ?
    // initiating class loader list
    // NOTE: for classes with low serialNumber, these are unused, and the
    // values are kept in a table in gDvm.
    // InitiatingLoaderList initiating_loader_list_;
    // The following data exist in real class objects.
    // Embedded Imtable, for class object that's not an interface, fixed size.
    uint32_t embedded_imtable_[0];
    // Embedded Vtable, for class object that's not an interface, variable size.
    uint32_t embedded_vtable_[0];
    // Static fields, variable size.
    uint32_t fields_[0];
};

class KClass:public ArtClass{
public:
    // static, private, and <init> methods
    uint32_t direct_methods_;
    // instance fields
    //
    // These describe the layout of the contents of an Object.
    // Note that only the fields directly declared by this class are
    // listed in ifields; fields declared by a superclass are listed in
    // the superclass's Class.ifields.
    //
    // All instance fields that refer to objects are guaranteed to be at
    // the beginning of the field list.  num_reference_instance_fields_
    // specifies the number of reference fields.
    uint32_t ifields_;
    // The interface table (iftable_) contains pairs of a interface class and an array of the
    // interface methods. There is one pair per interface supported by this class.  That means one
    // pair for each interface we support directly, indirectly via superclass, or indirectly via a
    // superinterface.  This will be null if neither we nor our superclass implement any interfaces.
    //
    // Why we need this: given "class Foo implements Face", declare "Face faceObj = new Foo()".
    // Invoke faceObj.blah(), where "blah" is part of the Face interface.  We can't easily use a
    // single vtable.
    //
    // For every interface a concrete class implements, we create an array of the concrete vtable_
    // methods for the methods in the interface.
    uint32_t iftable_;
    // Descriptor for the class such as "java.lang.Class" or "[C". Lazily initialized by ComputeName
    uint32_t name_;
    // Static fields
    uint32_t sfields_;
    // The superclass, or NULL if this is java.lang.Object, an interface or primitive type.
    uint32_t super_class_;
    // If class verify fails, we must return same error on subsequent tries.
    uint32_t verify_error_class_;
    // Virtual methods defined in this class; invoked through vtable.
    uint32_t virtual_methods_;
    // Virtual method table (vtable), for use by "invoke-virtual".  The vtable from the superclass is
    // copied in, and virtual methods from our class either replace those from the super or are
    // appended. For abstract classes, methods may be created in the vtable that aren't in
    // virtual_ methods_ for miranda methods.
    uint32_t vtable_;

    // Access flags; low 16 bits are defined by VM spec.
    uint32_t access_flags_;
    // Total size of the Class instance; used when allocating storage on gc heap.
    // See also object_size_.
    size_t class_size_;
    // Tid used to check for recursive <clinit> invocation.
    pid_t clinit_thread_id_;
    // ClassDef index in dex file, -1 if no class definition such as an array.
    // TODO: really 16bits
    int32_t dex_class_def_idx_;
    // Type index in dex file.
    // TODO: really 16bits
    int32_t dex_type_idx_;
    // Number of instance fields that are object refs.
    size_t num_reference_instance_fields_;
    // Number of static fields that are object refs,
    size_t num_reference_static_fields_;
    // Total object size; used when allocating storage on gc heap.
    // (For interfaces and abstract classes this will be zero.)
    // See also class_size_.
    size_t object_size_;
    // Primitive type value, or Primitive::kPrimNot (0); set for generated primitive classes.
    uint32_t primitive_type_;
    // Bitmap of offsets of ifields.
    uint32_t reference_instance_offsets_;
    // Bitmap of offsets of sfields.
    uint32_t reference_static_offsets_;
    // State of class initialization.
    int32_t status_;
    // TODO: ?
    // initiating class loader list
    // NOTE: for classes with low serialNumber, these are unused, and the
    // values are kept in a table in gDvm.
    // InitiatingLoaderList initiating_loader_list_;

    // Location of first static field.
    uint32_t fields_[0];
};

class ArtField final{
public:
    uint32_t declaring_class_;

    uint32_t access_flags_;

    // Dex cache index of field id
    uint32_t field_dex_idx_;

    // Offset of field within an instance or in the Class' static fields
    uint32_t offset_;

    u4 Size(){
        return sizeof(*this);
    }
};
class ArtFieldLollipop final : public Object {
public:
    uint32_t declaring_class_;

    uint32_t access_flags_;

    // Dex cache index of field id
    uint32_t field_dex_idx_;

    // Offset of field within an instance or in the Class' static fields
    uint32_t offset_;

    u4 Size(){
        return sizeof(*this);
    }
};

class ArtMethodLollipop final{
public:
// Field order required by test "ValidateFieldOrderOfJavaCppUnionClasses".
    // The class we are a part of
    uint32_t declaring_class_;

    // short cuts to declaring_class_->dex_cache_ member for fast compiled code access
    uint32_t dex_cache_resolved_methods_;

    // short cuts to declaring_class_->dex_cache_ member for fast compiled code access
    uint32_t dex_cache_resolved_types_;

    // short cuts to declaring_class_->dex_cache_ member for fast compiled code access
    uint32_t dex_cache_strings_;

    // Method dispatch from the interpreter invokes this pointer which may cause a bridge into
    // compiled code.
    uint64_t entry_point_from_interpreter_;

    // Pointer to JNI function registered to this method, or a function to resolve the JNI function.
    uint64_t entry_point_from_jni_;

    // Method dispatch from portable compiled code invokes this pointer which may cause bridging into
    // quick compiled code or the interpreter.
#ifdef ART_USE_PORTABLE_COMPILER
    uint64_t entry_point_from_portable_compiled_code_;
#endif

    // Method dispatch from quick compiled code invokes this pointer which may cause bridging into
    // portable compiled code or the interpreter.
    uint64_t entry_point_from_quick_compiled_code_;

    // Pointer to a data structure created by the compiler and used by the garbage collector to
    // determine which registers hold live references to objects within the heap. Keyed by native PC
    // offsets for the quick compiler and dex PCs for the portable.
    uint64_t gc_map_;

    // Access flags; low 16 bits are defined by spec.
    uint32_t access_flags_;

    /* Dex file fields. The defining dex file is available via declaring_class_->dex_cache_ */

    // Offset to the CodeItem.
    uint32_t dex_code_item_offset_;

    // Index into method_ids of the dex file associated with this method.
    uint32_t dex_method_index_;

    /* End of dex file fields. */

    // Entry within a dispatch table for this method. For static/direct methods the index is into
    // the declaringClass.directMethods, for virtual methods the vtable and for interface methods the
    // ifTable.
    uint32_t method_index_;

    u4 Size(){
        return sizeof(*this);
    }
};
class ArtMethodKitkat{
public:
    // Field order required by test "ValidateFieldOrderOfJavaCppUnionClasses".
    // The class we are a part of
    u4 declaring_class_;
    // short cuts to declaring_class_->dex_cache_ member for fast compiled code access
    void* dex_cache_initialized_static_storage_;
    // short cuts to declaring_class_->dex_cache_ member for fast compiled code access
    void* dex_cache_resolved_methods_;
    // short cuts to declaring_class_->dex_cache_ member for fast compiled code access
    void* dex_cache_resolved_types_;
    // short cuts to declaring_class_->dex_cache_ member for fast compiled code access
    void* dex_cache_strings_;
    // Access flags; low 16 bits are defined by spec.
    uint32_t access_flags_;
    // Offset to the CodeItem.
    uint32_t dex_code_item_offset_;
    // Architecture-dependent register spill mask
    uint32_t core_spill_mask_;
    // Compiled code associated with this method for callers from managed code.
    // May be compiled managed code or a bridge for invoking a native method.
    // TODO: Break apart this into portable and quick.
    const void* entry_point_from_compiled_code_;
    // Called by the interpreter to execute this method.
    void* entry_point_from_interpreter_;
    // Architecture-dependent register spill mask
    uint32_t fp_spill_mask_;
    // Total size in bytes of the frame
    size_t frame_size_in_bytes_;
    // Garbage collection map of native PC offsets (quick) or dex PCs (portable) to reference bitmaps.
    const uint8_t* gc_map_;
    // Mapping from native pc to dex pc
    const uint32_t* mapping_table_;
    // Index into method_ids of the dex file associated with this method
    uint32_t dex_method_index_;
    // For concrete virtual methods, this is the offset of the method in Class::vtable_.
    //
    // For abstract methods in an interface class, this is the offset of the method in
    // "iftable_->Get(n)->GetMethodArray()".
    //
    // For static and direct methods this is the index in the direct methods table.
    uint32_t method_index_;

    const void* native_method_;

    // When a register is promoted into a register, the spill mask holds which registers hold dex
    // registers. The first promoted register's corresponding dex register is vmap_table_[1], the Nth
    // is vmap_table_[N]. vmap_table_[0] holds the length of the table.
    const uint16_t* vmap_table_;

    u4 Size(){
        return sizeof(*this);
    }
};

class ArtMethodLollipopMr1 final : public Object {
public:
    uint32_t declaring_class_;

    // Short cuts to declaring_class_->dex_cache_ member for fast compiled code access.
    uint32_t dex_cache_resolved_methods_;

    // Short cuts to declaring_class_->dex_cache_ member for fast compiled code access.
    uint32_t dex_cache_resolved_types_;

    // Access flags; low 16 bits are defined by spec.
    uint32_t access_flags_;

    /* Dex file fields. The defining dex file is available via declaring_class_->dex_cache_ */

    // Offset to the CodeItem.
    uint32_t dex_code_item_offset_;

    // Index into method_ids of the dex file associated with this method.
    uint32_t dex_method_index_;

    /* End of dex file fields. */

    // Entry within a dispatch table for this method. For static/direct methods the index is into
    // the declaringClass.directMethods, for virtual methods the vtable and for interface methods the
    // ifTable.
    uint32_t method_index_;

    struct PtrSizedFields {
        // Method dispatch from the interpreter invokes this pointer which may cause a bridge into
        // compiled code.
        void* entry_point_from_interpreter_;
        // Pointer to JNI function registered to this method, or a function to resolve the JNI function.
        void* entry_point_from_jni_;
        // Method dispatch from quick compiled code invokes this pointer which may cause bridging into
        // portable compiled code or the interpreter.
        void* entry_point_from_quick_compiled_code_;
    } ptr_sized_fields_;

    u4 Size(){
        return sizeof(*this);
    }
};

class ArtMethodMarshmallow final {
public:
    uint32_t declaring_class_;

    // Short cuts to declaring_class_->dex_cache_ member for fast compiled code access.
    uint32_t dex_cache_resolved_methods_;

    // Short cuts to declaring_class_->dex_cache_ member for fast compiled code access.
    uint32_t dex_cache_resolved_types_;

    // Access flags; low 16 bits are defined by spec.
    uint32_t access_flags_;

    /* Dex file fields. The defining dex file is available via declaring_class_->dex_cache_ */

    // Offset to the CodeItem.
    uint32_t dex_code_item_offset_;

    // Index into method_ids of the dex file associated with this method.
    uint32_t dex_method_index_;

    /* End of dex file fields. */

    // Entry within a dispatch table for this method. For static/direct methods the index is into
    // the declaringClass.directMethods, for virtual methods the vtable and for interface methods the
    // ifTable.
    uint32_t method_index_;

    struct PtrSizedFields {
        // Method dispatch from the interpreter invokes this pointer which may cause a bridge into
        // compiled code.
        void* entry_point_from_interpreter_;
        // Pointer to JNI function registered to this method, or a function to resolve the JNI function.
        void* entry_point_from_jni_;
        // Method dispatch from quick compiled code invokes this pointer which may cause bridging into
        // the interpreter.
        void* entry_point_from_quick_compiled_code_;
    } ptr_sized_fields_;

    u4 Size(){
        return sizeof(*this);
    }
};
class ArtMethodNougat final{
public:

    // Field order required by test "ValidateFieldOrderOfJavaCppUnionClasses".
    // The class we are a part of.
    uint32_t declaring_class_;
    // Access flags; low 16 bits are defined by spec.
    uint32_t access_flags_;
    /* Dex file fields. The defining dex file is available via declaring_class_->dex_cache_ */
    // Offset to the CodeItem.
    uint32_t dex_code_item_offset_;
    // Index into method_ids of the dex file associated with this method.
    uint32_t dex_method_index_;
    /* End of dex file fields. */
    // Entry within a dispatch table for this method. For static/direct methods the index is into
    // the declaringClass.directMethods, for virtual methods the vtable and for interface methods the
    // ifTable.
    uint16_t method_index_;


    // The hotness we measure for this method. Incremented by the interpreter. Not atomic, as we allow
    // missing increments: if the method is hot, we will see it eventually.
    uint16_t hotness_count_;
    // Fake padding field gets inserted here.
    // Must be the last fields in the method.
    // PACKED(4) is necessary for the correctness of
    // RoundUp(OFFSETOF_MEMBER(ArtMethod, ptr_sized_fields_), pointer_size).
    struct PtrSizedFields {
        // Short cuts to declaring_class_->dex_cache_ member for fast compiled code access.
        ArtMethodNougat** dex_cache_resolved_methods_;

        // Short cuts to declaring_class_->dex_cache_ member for fast compiled code access.
        void* dex_cache_resolved_types_;

        // Pointer to JNI function registered to this method, or a function to resolve the JNI function,
        // or the profiling data for non-native methods, or an ImtConflictTable.
        void* entry_point_from_jni_;

        // Method dispatch from quick compiled code invokes this pointer which may cause bridging into
        // the interpreter.
        void* entry_point_from_quick_compiled_code_;
    } ptr_sized_fields_;

    inline u4 Size(){
        return sizeof(*this);
    }
};

#endif //HOOKMANAGER_ART_METHOD_H
