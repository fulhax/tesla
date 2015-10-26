#include "scriptstdstring.h"

#include <assert.h> // assert()
#include <stdio.h>  // sprintf()
#include <stdlib.h> // strtod()
#include <string.h> // strstr()
#include <locale.h> // setlocale()
#include <string>

#include <sstream>  // std::stringstream
#include <map>      // std::map

BEGIN_AS_NAMESPACE

#if AS_USE_STRINGPOOL == 1

// By keeping the literal strings in a pool the application
// performance is improved as there are less string copies created.

// The string pool will be kept as user data in the engine. We'll
// need a specific type to identify the string pool user data.
// We just define a number here that we assume nobody else is using for
// object type user data. The add-ons have reserved the numbers 1000
// through 1999 for this purpose, so we should be fine.
const asPWORD STRING_POOL = 1001;

// This global static variable is placed here rather than locally within the
// StringFactory, due to memory leak detectors that don't see the deallocation
// of global variables. By placing the variable globally it will be initialized
// before the memory leak detector starts, thus it won't report the missing
// deallocation. An example of this the Marmalade leak detector initialized with
// IwGxInit() and finished with IwGxTerminate().
static const std::string emptyString;

static const std::string &StringFactory(asUINT length, const char *s)
{
    // Each engine instance has its own string pool
    asIScriptContext *ctx = asGetActiveContext();

    if(ctx == 0) {
        // The string factory can only be called from a script
        assert(ctx);
        return emptyString;
    }

    asIScriptEngine *engine = ctx->GetEngine();

    // TODO: runtime optimize: Use unordered_map if C++11 is supported, i.e.
    // MSVC10+, gcc 4.?+
    auto pool = reinterpret_cast<std::map<const char *, std::string>*>(
                    engine->GetUserData(STRING_POOL));

    if(!pool) {
        // The string pool hasn't been created yet, so we'll create it now
        asAcquireExclusiveLock();

        // Make sure the string pool wasn't created while we were waiting for
        // the lock
        pool = reinterpret_cast<std::map<const char *, std::string>*>(
                   engine->GetUserData(STRING_POOL));

        if(!pool) {
            #if defined(AS_MARMALADE) || defined(MARMALADE)
            pool = new std::map<const char *, std::string>;
            #else
            pool = new(std::nothrow) std::map<const char *, std::string>;
            #endif

            if(pool == 0) {
                ctx->SetException("Out of memory");
                asReleaseExclusiveLock();
                return emptyString;
            }

            engine->SetUserData(pool, STRING_POOL);
        }

        asReleaseExclusiveLock();
    }

    // We can't let other threads modify the pool while we query it
    asAcquireSharedLock();

    // First check if a string object hasn't been created already
    auto it = pool->find(s);

    if(it != pool->end()) {
        asReleaseSharedLock();
        return it->second;
    }

    asReleaseSharedLock();

    // Acquire an exclusive lock so we can add the new string to the pool
    asAcquireExclusiveLock();

    // Make sure the string wasn't created while we were waiting for the
    // exclusive lock
    it = pool->find(s);

    if(it == pool->end()) {
        // Create a new string object
        it = pool->insert(
                 std::map<const char *, std::string>::value_type(
                     s,
                     std::string(s, length)
                 )).first;
    }

    asReleaseExclusiveLock();
    return it->second;
}

static void CleanupEngineStringPool(asIScriptEngine *engine)
{
    auto pool = reinterpret_cast<std::map<const char *, std::string>*>(
                    engine->GetUserData(STRING_POOL));

    if(pool) {
        delete pool;
    }
}

#else
static string StringFactory(asUINT length, const char *s)
{
    return string(s, length);
}
#endif

static void ConstructString(std::string *thisPointer)
{
    new(thisPointer) std::string();
}

static void CopyConstructString(const std::string &other,
                                std::string *thisPointer)
{
    new(thisPointer) std::string(other);
}

static void DestructString(std::string *thisPointer)
{
    thisPointer->~basic_string();
}

static std::string &AddAssignStringToString(const std::string &str,
        std::string *dest)
{
    // We don't register the method directly because some compilers
    // and standard libraries inline the definition, resulting in the
    // linker being unable to find the declaration.
    // Example: CLang/LLVM with XCode 4.3 on OSX 10.7
    *dest += str;
    return *dest;
}

// bool string::isEmpty()
// bool string::empty() // if AS_USE_STLNAMES == 1
static bool StringIsEmpty(const std::string &str)
{
    // We don't register the method directly because some compilers
    // and standard libraries inline the definition, resulting in the
    // linker being unable to find the declaration
    // Example: CLang/LLVM with XCode 4.3 on OSX 10.7
    return str.empty();
}

static std::string &AssignUIntToString(unsigned int i, std::string *dest)
{
    std::ostringstream stream;
    stream << i;
    *dest = stream.str();
    return *dest;
}

static std::string &AddAssignUIntToString(unsigned int i, std::string *dest)
{
    std::ostringstream stream;
    stream << i;
    *dest += stream.str();
    return *dest;
}

static std::string AddStringUInt(const std::string &str, unsigned int i)
{
    std::ostringstream stream;
    stream << i;
    return str + stream.str();
}

static std::string AddIntString(int i, const std::string &str)
{
    std::ostringstream stream;
    stream << i;
    return stream.str() + str;
}

static std::string &AssignIntToString(int i, std::string *dest)
{
    std::ostringstream stream;
    stream << i;
    *dest = stream.str();
    return *dest;
}

static std::string &AddAssignIntToString(int i, std::string *dest)
{
    std::ostringstream stream;
    stream << i;
    *dest += stream.str();
    return *dest;
}

static std::string AddStringInt(const std::string &str, int i)
{
    std::ostringstream stream;
    stream << i;
    return str + stream.str();
}

static std::string AddUIntString(unsigned int i, const std::string &str)
{
    std::ostringstream stream;
    stream << i;
    return stream.str() + str;
}

static std::string &AssignDoubleToString(double f, std::string *dest)
{
    std::ostringstream stream;
    stream << f;
    *dest = stream.str();
    return *dest;
}

static std::string &AddAssignDoubleToString(double f, std::string *dest)
{
    std::ostringstream stream;
    stream << f;
    *dest += stream.str();
    return *dest;
}

static std::string &AssignBoolToString(bool b, std::string *dest)
{
    std::ostringstream stream;
    stream << (b ? "true" : "false");
    *dest = stream.str();
    return *dest;
}

static std::string &AddAssignBoolToString(bool b, std::string *dest)
{
    std::ostringstream stream;
    stream << (b ? "true" : "false");
    *dest += stream.str();
    return *dest;
}

static std::string AddStringDouble(const std::string &str, double f)
{
    std::ostringstream stream;
    stream << f;
    return str + stream.str();
}

static std::string AddDoubleString(double f, const std::string &str)
{
    std::ostringstream stream;
    stream << f;
    return stream.str() + str;
}

static std::string AddStringBool(const std::string &str, bool b)
{
    std::ostringstream stream;
    stream << (b ? "true" : "false");
    return str + stream.str();
}

static std::string AddBoolString(bool b, const std::string &str)
{
    std::ostringstream stream;
    stream << (b ? "true" : "false");
    return stream.str() + str;
}

static const char *StringCharAt(unsigned int i, std::string *str)
{
    if(i >= str->size()) {
        // Set a script exception
        asIScriptContext *ctx = asGetActiveContext();
        ctx->SetException("Out of range");

        // Return a null pointer
        return 0;
    }

    return str[i].c_str();
}

// AngelScript signature:
// int string::opCmp(const string &in) const
static int StringCmp(const std::string &a, const std::string &b)
{
    int cmp = 0;

    if(a < b) {
        cmp = -1;
    } else if(a > b) {
        cmp = 1;
    }

    return cmp;
}

// This function returns the index of the first position where the substring
// exists in the input string. If the substring doesn't exist in the input
// string -1 is returned.
//
// AngelScript signature:
// int string::findFirst(const string &in sub, uint start = 0) const
static int StringFindFirst(const std::string &sub, asUINT start,
                           const std::string &str)
{
    // We don't register the method directly because the argument types change
    // between 32bit and 64bit platforms
    return static_cast<int>(str.find(sub, start));
}

// This function returns the index of the last position where the substring
// exists in the input string. If the substring doesn't exist in the input
// string -1 is returned.
//
// AngelScript signature:
// int string::findLast(const string &in sub, int start = -1) const
static int StringFindLast(const std::string &sub, int start,
                          const std::string &str)
{
    // We don't register the method directly because the argument types change
    // between 32bit and 64bit platforms
    return static_cast<int>(str.rfind(sub, static_cast<size_t>(start)));
}

// AngelScript signature:
// uint string::length() const
static asUINT StringLength(const std::string &str)
{
    // We don't register the method directly because the return type changes
    // between 32bit and 64bit platforms
    return static_cast<asUINT>(str.length());
}


// AngelScript signature:
// void string::resize(uint l)
static void StringResize(asUINT l, std::string *str)
{
    // We don't register the method directly because the argument types change
    // between 32bit and 64bit platforms
    str->resize(l);
}

// AngelScript signature:
// string formatInt(int64 val, const string &in options, uint width)
static std::string formatInt(asINT64 value, const std::string &options,
                             asUINT width)
{
    bool leftJustify = options.find("l") != std::string::npos;
    bool padWithZero = options.find("0") != std::string::npos;
    bool alwaysSign  = options.find("+") != std::string::npos;
    bool spaceOnSign = options.find(" ") != std::string::npos;
    bool hexSmall    = options.find("h") != std::string::npos;
    bool hexLarge    = options.find("H") != std::string::npos;

    std::string fmt = "%";

    if(leftJustify) {
        fmt += "-";
    }

    if(alwaysSign) {
        fmt += "+";
    }

    if(spaceOnSign) {
        fmt += " ";
    }

    if(padWithZero) {
        fmt += "0";
    }

    #ifdef __GNUC__
    #ifdef _LP64
    fmt += "*l";
    #else
    fmt += "*ll";
    #endif
    #else
    fmt += "*I64";
    #endif

    if(hexSmall) {
        fmt += "x";
    } else if(hexLarge) {
        fmt += "X";
    } else {
        fmt += "d";
    }

    std::string buf;
    buf.resize(width + 20);
    #if _MSC_VER >= 1400 && !defined(AS_MARMALADE) && !defined(MARMALADE)
    // MSVC 8.0 / 2005 or newer
    sprintf_s(&buf[0], buf.size(), fmt.c_str(), width, value);
    #else
    snprintf(&buf[0], buf.size(), fmt.c_str(), width, value);
    #endif
    buf.resize(strlen(&buf[0]));

    return buf;
}

// AngelScript signature:
// string formatFloat(double val, const string &in options, uint width, uint precision)
static std::string formatFloat(double value, const std::string &options,
                               asUINT width, asUINT precision)
{
    bool leftJustify = options.find("l") != std::string::npos;
    bool padWithZero = options.find("0") != std::string::npos;
    bool alwaysSign  = options.find("+") != std::string::npos;
    bool spaceOnSign = options.find(" ") != std::string::npos;
    bool expSmall    = options.find("e") != std::string::npos;
    bool expLarge    = options.find("E") != std::string::npos;

    std::string fmt = "%";

    if(leftJustify) {
        fmt += "-";
    }

    if(alwaysSign) {
        fmt += "+";
    }

    if(spaceOnSign) {
        fmt += " ";
    }

    if(padWithZero) {
        fmt += "0";
    }

    fmt += "*.*";

    if(expSmall) {
        fmt += "e";
    } else if(expLarge) {
        fmt += "E";
    } else {
        fmt += "f";
    }

    std::string buf;
    buf.resize(width + precision + 50);
    #if _MSC_VER >= 1400 && !defined(AS_MARMALADE) && !defined(MARMALADE)
    // MSVC 8.0 / 2005 or newer
    sprintf_s(&buf[0], buf.size(), fmt.c_str(), width, precision, value);
    #else
    snprintf(&buf[0], buf.size(), fmt.c_str(), width, precision, value);
    #endif
    buf.resize(strlen(&buf[0]));

    return buf;
}

// AngelScript signature:
// int64 parseInt(const string &in val, uint base = 10, uint &out byteCount = 0)
static asINT64 parseInt(const std::string &val, asUINT base,
                        asUINT *byteCount)
{
    // Only accept base 10 and 16
    if(base != 10 && base != 16) {
        if(byteCount) {
            *byteCount = 0;
        }

        return 0;
    }

    char *end;
    asINT64 res = strtol(val.c_str(), &end, base);

    if(byteCount) {
        *byteCount = asUINT(size_t(end - val.c_str()));
    }

    return res;
}

// AngelScript signature:
// double parseFloat(const string &in val, uint &out byteCount = 0)
double parseFloat(const std::string &val, asUINT *byteCount)
{
    char *end;

    // WinCE doesn't have setlocale. Some quick testing on my current
    // platform still manages to parse the numbers such as "3.14" even
    // if the decimal for the locale is ",".
    #if !defined(_WIN32_WCE) && !defined(ANDROID)
    // Set the locale to C so that we are guaranteed to parse the float value
    // correctly
    char *orig = setlocale(LC_NUMERIC, 0);
    setlocale(LC_NUMERIC, "C");
    #endif

    double res = strtod(val.c_str(), &end);

    #if !defined(_WIN32_WCE) && !defined(ANDROID)
    // Restore the locale
    setlocale(LC_NUMERIC, orig);
    #endif

    if(byteCount) {
        *byteCount = asUINT(size_t(end - val.c_str()));
    }

    return res;
}

// This function returns a string containing the substring of the input string
// determined by the starting index and count of characters.
//
// AngelScript signature:
// string string::substr(uint start = 0, int count = -1) const
static std::string StringSubString(asUINT start, int count,
                                   const std::string &str)
{
    // Check for out-of-bounds
    std::string ret;

    if(start < str.length() && count != 0) {
        ret = str.substr(start, count);
    }

    return ret;
}

// String equality comparison.
// Returns true iff lhs is equal to rhs.
//
// For some reason gcc 4.7 has difficulties resolving the
// asFUNCTIONPR(operator==, (const string &, const string &)
// makro, so this wrapper was introduced as work around.
static bool StringEquals(const std::string &lhs, const std::string &rhs)
{
    return lhs == rhs;
}

void RegisterStdString_Native(asIScriptEngine *engine)
{
    // Register the string type
    engine->RegisterObjectType(
        "string",
        sizeof(std::string),
        asOBJ_VALUE | asOBJ_APP_CLASS_CDAK);

    #if AS_USE_STRINGPOOL == 1
    // Register the string factory
    engine->RegisterStringFactory(
        "const string &",
        asFUNCTION(StringFactory),
        asCALL_CDECL);

    // Register the cleanup callback for the string pool
    engine->SetEngineUserDataCleanupCallback(
        CleanupEngineStringPool,
        STRING_POOL);

    #else
    // Register the string factory
    engine->RegisterStringFactory(
        "string",
        asFUNCTION(StringFactory),
        asCALL_CDECL);

    #endif

    // Register the object operator overloads
    engine->RegisterObjectBehaviour(
        "string",
        asBEHAVE_CONSTRUCT,
        "void f()",
        asFUNCTION(ConstructString),
        asCALL_CDECL_OBJLAST);

    engine->RegisterObjectBehaviour(
        "string",
        asBEHAVE_CONSTRUCT,
        "void f(const string &in)",
        asFUNCTION(CopyConstructString),
        asCALL_CDECL_OBJLAST);

    engine->RegisterObjectBehaviour(
        "string",
        asBEHAVE_DESTRUCT,
        "void f()",
        asFUNCTION(DestructString),
        asCALL_CDECL_OBJLAST);

    engine->RegisterObjectMethod(
        "string",
        "string &opAssign(const string &in)",
        asMETHODPR(
            std::string,
            operator =,
            (const std::string &),
            std::string &),
        asCALL_THISCALL);

    // Need to use a wrapper on Mac OS X 10.7/XCode 4.3 and CLang/LLVM,
    // otherwise the linker fails
    engine->RegisterObjectMethod(
        "string",
        "string &opAddAssign(const string &in)",
        asFUNCTION(AddAssignStringToString),
        asCALL_CDECL_OBJLAST);

    // engine->RegisterObjectMethod(
    //         "string",
    //         "string &opAddAssign(const string &in)",
    //         asMETHODPR(string, operator+=, (const string &), string &),
    //         asCALL_THISCALL);
    //
    //

    // Need to use a wrapper for operator== otherwise gcc 4.7 fails to compile
    engine->RegisterObjectMethod(
        "string",
        "bool opEquals(const string &in) const",
        asFUNCTIONPR(
            StringEquals,
            (const std::string &, const std::string &),
            bool),
        asCALL_CDECL_OBJFIRST);

    engine->RegisterObjectMethod(
        "string",
        "int opCmp(const string &in) const",
        asFUNCTION(StringCmp),
        asCALL_CDECL_OBJFIRST);

    engine->RegisterObjectMethod(
        "string",
        "string opAdd(const string &in) const",
        asFUNCTIONPR(
            std::operator +,
            (const std::string &, const std::string &),
            std::string),
        asCALL_CDECL_OBJFIRST);

    // The string length can be accessed through methods or through virtual property
    engine->RegisterObjectMethod(
        "string",
        "uint length() const",
        asFUNCTION(StringLength),
        asCALL_CDECL_OBJLAST);

    engine->RegisterObjectMethod(
        "string",
        "void resize(uint)",
        asFUNCTION(StringResize),
        asCALL_CDECL_OBJLAST);

    engine->RegisterObjectMethod(
        "string",
        "uint get_length() const",
        asFUNCTION(StringLength),
        asCALL_CDECL_OBJLAST);

    engine->RegisterObjectMethod(
        "string",
        "void set_length(uint)",
        asFUNCTION(StringResize),
        asCALL_CDECL_OBJLAST);

    // Need to use a wrapper on Mac OS X 10.7/XCode 4.3 and CLang/LLVM, otherwise the linker fails
    //  engine->RegisterObjectMethod("string", "bool isEmpty() const", asMETHOD(string, empty), asCALL_THISCALL); assert( r >= 0 );
    engine->RegisterObjectMethod(
        "string",
        "bool isEmpty() const",
        asFUNCTION(StringIsEmpty),
        asCALL_CDECL_OBJLAST);

    // Register the index operator, both as a mutator and as an inspector
    // Note that we don't register the operator[] directly, as it doesn't do bounds checking
    engine->RegisterObjectMethod(
        "string",
        "uint8 &opIndex(uint)",
        asFUNCTION(StringCharAt),
        asCALL_CDECL_OBJLAST);

    engine->RegisterObjectMethod(
        "string",
        "const uint8 &opIndex(uint) const",
        asFUNCTION(StringCharAt),
        asCALL_CDECL_OBJLAST);

    // Automatic conversion from values
    engine->RegisterObjectMethod(
        "string",
        "string &opAssign(double)",
        asFUNCTION(AssignDoubleToString),
        asCALL_CDECL_OBJLAST);

    engine->RegisterObjectMethod(
        "string",
        "string &opAddAssign(double)",
        asFUNCTION(AddAssignDoubleToString),
        asCALL_CDECL_OBJLAST);

    engine->RegisterObjectMethod(
        "string",
        "string opAdd(double) const",
        asFUNCTION(AddStringDouble),
        asCALL_CDECL_OBJFIRST);

    engine->RegisterObjectMethod(
        "string",
        "string opAdd_r(double) const",
        asFUNCTION(AddDoubleString),
        asCALL_CDECL_OBJLAST);

    engine->RegisterObjectMethod(
        "string",
        "string &opAssign(int)",
        asFUNCTION(AssignIntToString),
        asCALL_CDECL_OBJLAST);

    engine->RegisterObjectMethod(
        "string",
        "string &opAddAssign(int)",
        asFUNCTION(AddAssignIntToString),
        asCALL_CDECL_OBJLAST);

    engine->RegisterObjectMethod(
        "string",
        "string opAdd(int) const",
        asFUNCTION(AddStringInt),
        asCALL_CDECL_OBJFIRST);

    engine->RegisterObjectMethod(
        "string",
        "string opAdd_r(int) const",
        asFUNCTION(AddIntString),
        asCALL_CDECL_OBJLAST);

    engine->RegisterObjectMethod(
        "string",
        "string &opAssign(uint)",
        asFUNCTION(AssignUIntToString),
        asCALL_CDECL_OBJLAST);

    engine->RegisterObjectMethod(
        "string",
        "string &opAddAssign(uint)",
        asFUNCTION(AddAssignUIntToString),
        asCALL_CDECL_OBJLAST);

    engine->RegisterObjectMethod(
        "string",
        "string opAdd(uint) const",
        asFUNCTION(AddStringUInt),
        asCALL_CDECL_OBJFIRST);

    engine->RegisterObjectMethod(
        "string",
        "string opAdd_r(uint) const",
        asFUNCTION(AddUIntString),
        asCALL_CDECL_OBJLAST);

    engine->RegisterObjectMethod(
        "string",
        "string &opAssign(bool)",
        asFUNCTION(AssignBoolToString),
        asCALL_CDECL_OBJLAST);

    engine->RegisterObjectMethod(
        "string",
        "string &opAddAssign(bool)",
        asFUNCTION(AddAssignBoolToString),
        asCALL_CDECL_OBJLAST);

    engine->RegisterObjectMethod(
        "string",
        "string opAdd(bool) const",
        asFUNCTION(AddStringBool),
        asCALL_CDECL_OBJFIRST);

    engine->RegisterObjectMethod(
        "string",
        "string opAdd_r(bool) const",
        asFUNCTION(AddBoolString),
        asCALL_CDECL_OBJLAST);

    // Utilities
    engine->RegisterObjectMethod(
        "string",
        "string substr(uint start = 0, int count = -1) const",
        asFUNCTION(StringSubString),
        asCALL_CDECL_OBJLAST);

    engine->RegisterObjectMethod(
        "string",
        "int findFirst(const string &in, uint start = 0) const",
        asFUNCTION(StringFindFirst),
        asCALL_CDECL_OBJLAST);

    engine->RegisterObjectMethod(
        "string",
        "int findLast(const string &in, int start = -1) const",
        asFUNCTION(StringFindLast),
        asCALL_CDECL_OBJLAST);

    engine->RegisterGlobalFunction(
        "string formatInt(int64 val, const string &in options, uint width = 0)",
        asFUNCTION(formatInt),
        asCALL_CDECL);

    engine->RegisterGlobalFunction(
        "string formatFloat(double val, const string &in options, uint width = 0, uint precision = 0)",
        asFUNCTION(formatFloat),
        asCALL_CDECL);

    engine->RegisterGlobalFunction(
        "int64 parseInt(const string &in, uint base = 10, uint &out byteCount = 0)",
        asFUNCTION(parseInt),
        asCALL_CDECL);

    engine->RegisterGlobalFunction(
        "double parseFloat(const string &in, uint &out byteCount = 0)",
        asFUNCTION(parseFloat),
        asCALL_CDECL);

    #if AS_USE_STLNAMES == 1
    // Same as length
    engine->RegisterObjectMethod(
        "string",
        "uint size() const",
        asFUNCTION(StringLength),
        asCALL_CDECL_OBJLAST);

    // Same as isEmpty
    engine->RegisterObjectMethod(
        "string",
        "bool empty() const",
        asFUNCTION(StringIsEmpty),
        asCALL_CDECL_OBJLAST);

    // Same as findFirst
    engine->RegisterObjectMethod(
        "string",
        "int find(const string &in, uint start = 0) const",
        asFUNCTION(StringFindFirst),
        asCALL_CDECL_OBJLAST);

    // Same as findLast
    engine->RegisterObjectMethod(
        "string",
        "int rfind(const string &in, int start = -1) const",
        asFUNCTION(StringFindLast),
        asCALL_CDECL_OBJLAST);
    #endif

    // TODO: Implement the following
    // findFirstOf
    // findLastOf
    // findFirstNotOf
    // findLastNotOf
    // findAndReplace - replaces a text found in the string
    // replaceRange - replaces a range of bytes in the string
    // trim/trimLeft/trimRight
    // multiply/times/opMul/opMul_r - takes the string and multiplies it n times, e.g. "-".multiply(5) returns "-----"
}

#if AS_USE_STRINGPOOL == 1
static void StringFactoryGeneric(asIScriptGeneric *gen)
{
    asUINT length = gen->GetArgDWord(0);
    const char *s = reinterpret_cast<const char *>(gen->GetArgAddress(1));

    // Return a reference to a string
    gen->SetReturnAddress(const_cast<std::string *>(&StringFactory(length, s)));
}
#else
static void StringFactoryGeneric(asIScriptGeneric *gen)
{
    asUINT length = gen->GetArgDWord(0);
    const char *s = (const char *)gen->GetArgAddress(1);

    // Return a string value
    new(gen->GetAddressOfReturnLocation()) std::string(StringFactory(length, s));
}
#endif

static void ConstructStringGeneric(asIScriptGeneric *gen)
{
    new(gen->GetObject()) std::string();
}

static void CopyConstructStringGeneric(asIScriptGeneric *gen)
{
    std::string *a = static_cast<std::string *>(gen->GetArgObject(0));
    new(gen->GetObject()) std::string(*a);
}

static void DestructStringGeneric(asIScriptGeneric *gen)
{
    std::string *ptr = static_cast<std::string *>(gen->GetObject());
    ptr->~basic_string();
}

static void AssignStringGeneric(asIScriptGeneric *gen)
{
    std::string *a = static_cast<std::string *>(gen->GetArgObject(0));
    std::string *self = static_cast<std::string *>(gen->GetObject());
    *self = *a;
    gen->SetReturnAddress(self);
}

static void AddAssignStringGeneric(asIScriptGeneric *gen)
{
    std::string *a = static_cast<std::string *>(gen->GetArgObject(0));
    std::string *self = static_cast<std::string *>(gen->GetObject());
    *self += *a;
    gen->SetReturnAddress(self);
}

static void StringEqualsGeneric(asIScriptGeneric *gen)
{
    std::string *a = static_cast<std::string *>(gen->GetObject());
    std::string *b = static_cast<std::string *>(gen->GetArgAddress(0));
    *reinterpret_cast<bool *>(gen->GetAddressOfReturnLocation()) = (*a == *b);
}

static void StringCmpGeneric(asIScriptGeneric *gen)
{
    std::string *a = static_cast<std::string *>(gen->GetObject());
    std::string *b = static_cast<std::string *>(gen->GetArgAddress(0));

    int cmp = 0;

    if(*a < *b) {
        cmp = -1;
    } else if(*a > *b) {
        cmp = 1;
    }

    *reinterpret_cast<int *>(gen->GetAddressOfReturnLocation()) = cmp;
}

static void StringAddGeneric(asIScriptGeneric *gen)
{
    std::string *a = static_cast<std::string *>(gen->GetObject());
    std::string *b = static_cast<std::string *>(gen->GetArgAddress(0));
    std::string ret_val = *a + *b;
    gen->SetReturnObject(&ret_val);
}

static void StringLengthGeneric(asIScriptGeneric *gen)
{
    std::string *self = static_cast<std::string *>(gen->GetObject());

    *static_cast<asUINT *>(gen->GetAddressOfReturnLocation()) =
        static_cast<asUINT>(self->length());
}

static void StringResizeGeneric(asIScriptGeneric *gen)
{
    std::string *self = static_cast<std::string *>(gen->GetObject());
    self->resize(*static_cast<asUINT *>(gen->GetAddressOfArg(0)));
}

static void StringCharAtGeneric(asIScriptGeneric *gen)
{
    unsigned int index = gen->GetArgDWord(0);
    std::string *self = static_cast<std::string *>(gen->GetObject());

    if(index >= self->size()) {
        // Set a script exception
        asIScriptContext *ctx = asGetActiveContext();
        ctx->SetException("Out of range");

        gen->SetReturnAddress(0);
    } else {
        gen->SetReturnAddress(&(self->operator [](index)));
    }
}

static void AssignInt2StringGeneric(asIScriptGeneric *gen)
{
    int *a = static_cast<int *>(gen->GetAddressOfArg(0));
    std::string *self = static_cast<std::string *>(gen->GetObject());
    std::stringstream sstr;
    sstr << *a;
    *self = sstr.str();
    gen->SetReturnAddress(self);
}

static void AssignUInt2StringGeneric(asIScriptGeneric *gen)
{
    unsigned int *a = static_cast<unsigned int *>(gen->GetAddressOfArg(0));
    std::string *self = static_cast<std::string *>(gen->GetObject());
    std::stringstream sstr;
    sstr << *a;
    *self = sstr.str();
    gen->SetReturnAddress(self);
}

static void AssignDouble2StringGeneric(asIScriptGeneric *gen)
{
    double *a = static_cast<double *>(gen->GetAddressOfArg(0));
    std::string *self = static_cast<std::string *>(gen->GetObject());
    std::stringstream sstr;
    sstr << *a;
    *self = sstr.str();
    gen->SetReturnAddress(self);
}

static void AssignBool2StringGeneric(asIScriptGeneric *gen)
{
    bool *a = static_cast<bool *>(gen->GetAddressOfArg(0));
    std::string *self = static_cast<std::string *>(gen->GetObject());
    std::stringstream sstr;
    sstr << (*a ? "true" : "false");
    *self = sstr.str();
    gen->SetReturnAddress(self);
}

static void AddAssignDouble2StringGeneric(asIScriptGeneric *gen)
{
    double *a = static_cast<double *>(gen->GetAddressOfArg(0));
    std::string *self = static_cast<std::string *>(gen->GetObject());
    std::stringstream sstr;
    sstr << *a;
    *self += sstr.str();
    gen->SetReturnAddress(self);
}

static void AddAssignInt2StringGeneric(asIScriptGeneric *gen)
{
    int *a = static_cast<int *>(gen->GetAddressOfArg(0));
    std::string *self = static_cast<std::string *>(gen->GetObject());
    std::stringstream sstr;
    sstr << *a;
    *self += sstr.str();
    gen->SetReturnAddress(self);
}

static void AddAssignUInt2StringGeneric(asIScriptGeneric *gen)
{
    unsigned int *a = static_cast<unsigned int *>(gen->GetAddressOfArg(0));
    std::string *self = static_cast<std::string *>(gen->GetObject());
    std::stringstream sstr;
    sstr << *a;
    *self += sstr.str();
    gen->SetReturnAddress(self);
}

static void AddAssignBool2StringGeneric(asIScriptGeneric *gen)
{
    bool *a = static_cast<bool *>(gen->GetAddressOfArg(0));
    std::string *self = static_cast<std::string *>(gen->GetObject());
    std::stringstream sstr;
    sstr << (*a ? "true" : "false");
    *self += sstr.str();
    gen->SetReturnAddress(self);
}

static void AddString2DoubleGeneric(asIScriptGeneric *gen)
{
    std::string *a = static_cast<std::string *>(gen->GetObject());
    double *b = static_cast<double *>(gen->GetAddressOfArg(0));
    std::stringstream sstr;
    sstr << *a << *b;
    std::string ret_val = sstr.str();
    gen->SetReturnObject(&ret_val);
}

static void AddString2IntGeneric(asIScriptGeneric *gen)
{
    std::string *a = static_cast<std::string *>(gen->GetObject());
    int *b = static_cast<int *>(gen->GetAddressOfArg(0));
    std::stringstream sstr;
    sstr << *a << *b;
    std::string ret_val = sstr.str();
    gen->SetReturnObject(&ret_val);
}

static void AddString2UIntGeneric(asIScriptGeneric *gen)
{
    std::string *a = static_cast<std::string *>(gen->GetObject());
    unsigned int *b = static_cast<unsigned int *>(gen->GetAddressOfArg(0));
    std::stringstream sstr;
    sstr << *a << *b;
    std::string ret_val = sstr.str();
    gen->SetReturnObject(&ret_val);
}

static void AddString2BoolGeneric(asIScriptGeneric *gen)
{
    std::string *a = static_cast<std::string *>(gen->GetObject());
    bool *b = static_cast<bool *>(gen->GetAddressOfArg(0));
    std::stringstream sstr;
    sstr << *a << (*b ? "true" : "false");
    std::string ret_val = sstr.str();
    gen->SetReturnObject(&ret_val);
}

static void AddDouble2StringGeneric(asIScriptGeneric *gen)
{
    double *a = static_cast<double *>(gen->GetAddressOfArg(0));
    std::string *b = static_cast<std::string *>(gen->GetObject());
    std::stringstream sstr;
    sstr << *a << *b;
    std::string ret_val = sstr.str();
    gen->SetReturnObject(&ret_val);
}

static void AddInt2StringGeneric(asIScriptGeneric *gen)
{
    int *a = static_cast<int *>(gen->GetAddressOfArg(0));
    std::string *b = static_cast<std::string *>(gen->GetObject());
    std::stringstream sstr;
    sstr << *a << *b;
    std::string ret_val = sstr.str();
    gen->SetReturnObject(&ret_val);
}

static void AddUInt2StringGeneric(asIScriptGeneric *gen)
{
    unsigned int *a = static_cast<unsigned int *>(gen->GetAddressOfArg(0));
    std::string *b = static_cast<std::string *>(gen->GetObject());
    std::stringstream sstr;
    sstr << *a << *b;
    std::string ret_val = sstr.str();
    gen->SetReturnObject(&ret_val);
}

static void AddBool2StringGeneric(asIScriptGeneric *gen)
{
    bool *a = static_cast<bool *>(gen->GetAddressOfArg(0));
    std::string *b = static_cast<std::string *>(gen->GetObject());
    std::stringstream sstr;
    sstr << (*a ? "true" : "false") << *b;
    std::string ret_val = sstr.str();
    gen->SetReturnObject(&ret_val);
}

static void StringSubString_Generic(asIScriptGeneric *gen)
{
    // Get the arguments
    std::string *str   = static_cast<std::string *>(gen->GetObject());
    asUINT  start = *static_cast<int *>(gen->GetAddressOfArg(0));
    int     count = *static_cast<int *>(gen->GetAddressOfArg(1));

    // Return the substring
    new(gen->GetAddressOfReturnLocation())
    std::string(StringSubString(start, count, *str));
}

void RegisterStdString_Generic(asIScriptEngine *engine)
{
    // Register the string type
    engine->RegisterObjectType(
            "string",
            sizeof(std::string),
            asOBJ_VALUE | asOBJ_APP_CLASS_CDAK);

    #if AS_USE_STRINGPOOL == 1
    // Register the string factory
    engine->RegisterStringFactory(
            "const string &",
            asFUNCTION(StringFactoryGeneric),
            asCALL_GENERIC);

    // Register the cleanup callback for the string pool
    engine->SetEngineUserDataCleanupCallback(
        CleanupEngineStringPool,
        STRING_POOL);

    #else
    // Register the string factory
    engine->RegisterStringFactory(
            "string",
            asFUNCTION(StringFactoryGeneric),
            asCALL_GENERIC);
    #endif

    // Register the object operator overloads
    engine->RegisterObjectBehaviour(
            "string",
            asBEHAVE_CONSTRUCT,
            "void f()",
            asFUNCTION(ConstructStringGeneric),
            asCALL_GENERIC);

    engine->RegisterObjectBehaviour(
            "string",
            asBEHAVE_CONSTRUCT,
            "void f(const string &in)",
            asFUNCTION(CopyConstructStringGeneric),
            asCALL_GENERIC);

    engine->RegisterObjectBehaviour(
            "string",
            asBEHAVE_DESTRUCT,
            "void f()",
            asFUNCTION(DestructStringGeneric),
            asCALL_GENERIC);

    engine->RegisterObjectMethod(
            "string",
            "string &opAssign(const string &in)",
            asFUNCTION(AssignStringGeneric),
            asCALL_GENERIC);

    engine->RegisterObjectMethod(
            "string",
            "string &opAddAssign(const string &in)",
            asFUNCTION(AddAssignStringGeneric),
            asCALL_GENERIC);

    engine->RegisterObjectMethod(
            "string",
            "bool opEquals(const string &in) const",
            asFUNCTION(StringEqualsGeneric),
            asCALL_GENERIC);

    engine->RegisterObjectMethod(
            "string",
            "int opCmp(const string &in) const",
            asFUNCTION(StringCmpGeneric),
            asCALL_GENERIC);

    engine->RegisterObjectMethod(
            "string",
            "string opAdd(const string &in) const",
            asFUNCTION(StringAddGeneric),
            asCALL_GENERIC);

    // Register the object methods
    engine->RegisterObjectMethod(
            "string",
            "uint length() const",
            asFUNCTION(StringLengthGeneric),
            asCALL_GENERIC);

    engine->RegisterObjectMethod(
            "string",
            "void resize(uint)",
            asFUNCTION(StringResizeGeneric),
            asCALL_GENERIC);

    // Register the index operator, both as a mutator and as an inspector
    engine->RegisterObjectMethod(
            "string",
            "uint8 &opIndex(uint)",
            asFUNCTION(StringCharAtGeneric),
            asCALL_GENERIC);

    engine->RegisterObjectMethod(
            "string",
            "const uint8 &opIndex(uint) const",
            asFUNCTION(StringCharAtGeneric),
            asCALL_GENERIC);

    // Automatic conversion from values
    engine->RegisterObjectMethod(
            "string",
            "string &opAssign(double)",
            asFUNCTION(AssignDouble2StringGeneric),
            asCALL_GENERIC);

    engine->RegisterObjectMethod(
            "string",
            "string &opAddAssign(double)",
            asFUNCTION(AddAssignDouble2StringGeneric),
            asCALL_GENERIC);

    engine->RegisterObjectMethod(
            "string",
            "string opAdd(double) const",
            asFUNCTION(AddString2DoubleGeneric),
            asCALL_GENERIC);

    engine->RegisterObjectMethod(
            "string",
            "string opAdd_r(double) const",
            asFUNCTION(AddDouble2StringGeneric),
            asCALL_GENERIC);

    engine->RegisterObjectMethod(
            "string",
            "string &opAssign(int)",
            asFUNCTION(AssignInt2StringGeneric),
            asCALL_GENERIC);

    engine->RegisterObjectMethod(
            "string",
            "string &opAddAssign(int)",
            asFUNCTION(AddAssignInt2StringGeneric),
            asCALL_GENERIC);

    engine->RegisterObjectMethod(
            "string",
            "string opAdd(int) const",
            asFUNCTION(AddString2IntGeneric),
            asCALL_GENERIC);

    engine->RegisterObjectMethod(
            "string",
            "string opAdd_r(int) const",
            asFUNCTION(AddInt2StringGeneric),
            asCALL_GENERIC);

    engine->RegisterObjectMethod(
            "string",
            "string &opAssign(uint)",
            asFUNCTION(AssignUInt2StringGeneric),
            asCALL_GENERIC);

    engine->RegisterObjectMethod(
            "string",
            "string &opAddAssign(uint)",
            asFUNCTION(AddAssignUInt2StringGeneric),
            asCALL_GENERIC);

    engine->RegisterObjectMethod(
            "string",
            "string opAdd(uint) const",
            asFUNCTION(AddString2UIntGeneric),
            asCALL_GENERIC);

    engine->RegisterObjectMethod(
            "string",
            "string opAdd_r(uint) const",
            asFUNCTION(AddUInt2StringGeneric),
            asCALL_GENERIC);

    engine->RegisterObjectMethod(
            "string",
            "string &opAssign(bool)",
            asFUNCTION(AssignBool2StringGeneric),
            asCALL_GENERIC);

    engine->RegisterObjectMethod(
            "string",
            "string &opAddAssign(bool)",
            asFUNCTION(AddAssignBool2StringGeneric),
            asCALL_GENERIC);

    engine->RegisterObjectMethod(
            "string",
            "string opAdd(bool) const",
            asFUNCTION(AddString2BoolGeneric),
            asCALL_GENERIC);

    engine->RegisterObjectMethod(
            "string",
            "string opAdd_r(bool) const",
            asFUNCTION(AddBool2StringGeneric),
            asCALL_GENERIC);

    engine->RegisterObjectMethod(
            "string",
            "string substr(uint start = 0, int count = -1) const",
            asFUNCTION(StringSubString_Generic),
            asCALL_GENERIC);
}

void RegisterStdString(asIScriptEngine *engine)
{
    if(strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY")) {
        RegisterStdString_Generic(engine);
    } else {
        RegisterStdString_Native(engine);
    }
}

END_AS_NAMESPACE
