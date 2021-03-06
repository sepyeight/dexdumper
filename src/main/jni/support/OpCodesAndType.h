//
// Created by Karven on 2016/10/17.
//

#ifndef DEXDUMP_OPCODESANDTYPE_H
#define DEXDUMP_OPCODESANDTYPE_H
enum RegisterType{
    TypePrimitiveExtend = UNDEFINED - 4,
    TypePrimitive = UNDEFINED - 3,
    TypeException = UNDEFINED - 2,
    TypeZero = UNDEFINED - 1,
};
enum FileOpCode {
    nop=0,
    move=0x1,
    moveF16=0x2,
    move16=0x3,
    moveW=0x4,
    moveF16W=0x5,
    move16W=0x6,
    moveOb=0x7,
    moveObF16=0x8,
    moveOb16=0x9,
    moveResult=0xa,
    moveResultW=0xb,
    moveResultOb=0xc,
    moveExcept=0xd,
    returnV=0xe,
    returnD=0xf,
    returnW=0x10,
    returnOb=0x11,
    const4=0x12,
    const16=0x13,
    const32=0x14,
    const16H=0x15,
    const16W=0x16,
    const32W=0x17,
    const64W=0x18,
    const16HW=0x19,
    constStr=0x1a,
    constStrJ=0x1b,
    constClass=0x1c,
    monitorEnter=0x1d,
    monitorExit=0x1e,
    checkCast=0x1f,
    instanceOf=0x20,
    arrayLen=0x21,
    newInstance=0x22,
    newArray=0x23,
    fillArray=0x24,
    fillArrayR=0x25,
    fillArrayData=0x26,
    throwD=0x27,
    goto8=0x28,
    goto16=0x29,
    goto32=0x2a,
    packedSwitch=0x2b,
    sparseSwitch=0x2c,
    //cmp-stub 2d-31
    ifEq=0x32,
    ifNe=0x33,
    ifLt=0x34,
    ifGe=0x35,
    ifGt=0x36,
    ifLe=0x37,
    ifEqz=0x38,
    ifNez=0x39,
    ifLtz=0x3a,
    ifGez=0x3b,
    ifGtz=0x3c,
    ifLez=0x3d,
    //stub 3e-43 unused
    aget=0x44,
    agetW=0x45,
    agetOb=0x46,
    agetBoolean=0x47,
    agetByte=0x48,
    agetChar=0x49,
    agetShort=0x4a,
    aput=0x4b,
    aputW=0x4c,
    aputOb=0x4d,
    aputBoolean=0x4e,
    aputByte=0x4f,
    aputChar=0x50,
    aputShort=0x51,
    iget=0x52,
    igetW=0x53,
    igetOb=0x54,
    igetBoolean=0x55,
    igetByte=0x56,
    igetChar=0x57,
    igetShort=0x58,
    iput=0x59,
    iputW=0x5a,
    iputOb=0x5b,
    iputBoolean=0x5c,
    iputByte=0x5d,
    iputChar=0x5e,
    iputShort=0x5f,
    sget=0x60,
    sgetW=0x61,
    sgetOb=0x62,
    sgetBoolean=0x63,
    sgetByte=0x64,
    sgetChar=0x65,
    sgetShort=0x66,
    sput=0x67,
    sputW=0x68,
    sputOb=0x69,
    sputBoolean=0x6a,
    sputByte=0x6b,
    sputChar=0x6c,
    sputShort=0x6d,
    invokeVirtual=0x6e,
    invokeSuper=0x6f,
    invokeDirect=0x70,
    invokeStatic=0x71,
    invokeInterface=0x72,
    returnVNo=0x73,
    invokeVirtualR=0x74,
    invokeSuperR=0x75,
    invokeDirectR=0x76,
    invokeStaticR=0x77,
    invokeInterfaceR=0x78,

    //stub 79-7a unused
    //stub 7b-8f change-prim-type
    //stub 90-af 8 bit computation
    //stub b0-cf 4 bit computation
    //stub d0-d7 16 bit constant computation
    //stub d8-e2 8 bit constant computation
};
enum ArtOpCode {
    igetQ=0xe3,
    igetWQ=0xe4,
    igetObQ=0xe5,
    iputQ=0xe6,
    iputWQ=0xe7,
    iputObQ=0xe8,
    invokeVirtualQ=0xe9,
    invokeVirtualRQ=0xea,
    iputBooleanQ=0xeb,
    iputByteQ=0xec,
    iputCharQ=0xed,
    iputShortQ=0xee,
    igetBooleanQ=0xef,
    igetByteQ=0xf0,
    igetCharQ=0xf1,
    igetShortQ=0xf2,
    invokeLambda=0xf3,
    //unused 0xf4
     captureVariable=0xf5,
    createLambda=0xf6,
    liberateVariable =0xf7,
    boxLambda=0xf8,
    unboxLambda=0xf9,
};
enum DalvikOpCode {
    OP_IGET_VOLATILE = 0xe3,
    OP_IPUT_VOLATILE = 0xe4,
    OP_SGET_VOLATILE = 0xe5,
    OP_SPUT_VOLATILE = 0xe6,
    OP_IGET_OBJECT_VOLATILE = 0xe7,
    OP_IGET_WIDE_VOLATILE = 0xe8,
    OP_IPUT_WIDE_VOLATILE = 0xe9,
    OP_SGET_WIDE_VOLATILE = 0xea,
    OP_SPUT_WIDE_VOLATILE = 0xeb,
    OP_BREAKPOINT = 0xec,
    OP_THROW_VERIFICATION_ERROR = 0xed,
    OP_EXECUTE_INLINE = 0xee,
    OP_EXECUTE_INLINE_RANGE = 0xef,
    OP_INVOKE_OBJECT_INIT_RANGE = 0xf0,
    OP_RETURN_VOID_BARRIER = 0xf1,
    OP_IGET_QUICK = 0xf2,
    OP_IGET_WIDE_QUICK = 0xf3,
    OP_IGET_OBJECT_QUICK = 0xf4,
    OP_IPUT_QUICK = 0xf5,
    OP_IPUT_WIDE_QUICK = 0xf6,
    OP_IPUT_OBJECT_QUICK = 0xf7,
    OP_INVOKE_VIRTUAL_QUICK = 0xf8,
    OP_INVOKE_VIRTUAL_QUICK_RANGE = 0xf9,
    OP_INVOKE_SUPER_QUICK = 0xfa,
    OP_INVOKE_SUPER_QUICK_RANGE = 0xfb,
    OP_IPUT_OBJECT_VOLATILE = 0xfc,
    OP_SGET_OBJECT_VOLATILE = 0xfd,
    OP_SPUT_OBJECT_VOLATILE = 0xfe,
};
#endif //DEXDUMP_OPCODESANDTYPE_H
