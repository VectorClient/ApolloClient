#pragma once

namespace mce
{
    enum class ImageUsage : unsigned char {
        Unknown = 0x0,
        sRGB = 0x1,
        Data = 0x2,
    };

    enum class ImageFormat : int {
        Unknown = 0x0,
        R8Unorm = 0x1,
        RGB8Unorm = 0x2,
        RGBA8Unorm = 0x3,
    };

    class Blob
    {
    public:
        using value_type = unsigned char;
        using size_type = size_t;
        using pointer = value_type*;
        using iterator = value_type*;
        using const_pointer = value_type const*;
        using const_iterator = value_type const*;

        using delete_function = void (*)(pointer);

        struct Deleter {
        public:
            delete_function mFn;

            void operator()(pointer x) const { mFn(x); }
        };

        using pointer_type = std::unique_ptr<value_type[], Deleter>;

        pointer_type mBlob{}; // this+0x0
        size_type    mSize{}; // this+0x10

        [[nodiscard]] _CONSTEXPR23 Blob() = default;

        [[nodiscard]] _CONSTEXPR23 Blob(std::span<unsigned char> s, Deleter deleter = {}) : mSize(s.size()) { // NOLINT
            mBlob = pointer_type(new value_type[mSize], deleter);
            std::copy(s.begin(), s.end(), mBlob.get());
        }

        [[nodiscard]] _CONSTEXPR23 pointer data() const { return mBlob.get(); }

        [[nodiscard]] _CONSTEXPR23 size_type size() const { return mSize; }

        [[nodiscard]] _CONSTEXPR23 std::span<unsigned char> view() const { return { data(), size() }; }

        [[nodiscard]] _CONSTEXPR23 Blob(Blob&&) noexcept = default;

        [[nodiscard]] _CONSTEXPR23 Blob(Blob const& other) : Blob(other.view(), other.mBlob.get_deleter()) {}

        _CONSTEXPR23 Blob& operator=(Blob const& other) {
            if (this != &other) {
                *this = Blob{ other };
            }
            return *this;
        }
    };

    struct Image
    {
    public:
        ImageFormat imageFormat{}; // this+0x0
        unsigned int        mWidth{};      // this+0x4
        unsigned int        mHeight{};     // this+0x8
        unsigned int        mDepth{};      // this+0xC
        ImageUsage  mUsage{};      // this+0x10
        Blob        mImageBytes;   // this+0x18
    };
}


namespace persona {
    enum class AnimatedTextureType : int {
        None = 0x0,
        Face = 0x1,
        Body32x32 = 0x2,
        Body128x128 = 0x3,
    };

    enum class AnimationExpression : int {
        Linear = 0x0,
        Blinking = 0x1,
    };


    enum class PieceType : int {
        Unknown = 0x0,
        Skeleton = 0x1,
        Body = 0x2,
        Skin = 0x3,
        Bottom = 0x4,
        Feet = 0x5,
        Dress = 0x6,
        Top = 0x7,
        High_Pants = 0x8,
        Hands = 0x9,
        Outerwear = 0xA,
        FacialHair = 0xB,
        Mouth = 0xC,
        Eyes = 0xD,
        Hair = 0xE,
        Hood = 0xF,
        Back = 0x10,
        FaceAccessory = 0x11,
        Head = 0x12,
        Legs = 0x13,
        LeftLeg = 0x14,
        RightLeg = 0x15,
        Arms = 0x16,
        LeftArm = 0x17,
        RightArm = 0x18,
        Capes = 0x19,
        ClassicSkin = 0x1A,
        Emote = 0x1B,
        Unsupported = 0x1C,
        Count = 0x1D,
    };


    class ArmSize
    {
    public:
        // ArmSize inner types define
        enum class Type : int64_t {
            Slim = 0,
            Wide = 1,
            Count = 2,
            Unknown = 3,
        };
    };
};

class AnimatedImageData
{
public:
    persona::AnimatedTextureType mType;
    persona::AnimationExpression mAnimationExpression;
    mce::Image                   mImage;
    float                        mFrames;
};

class SemVersion {
    unsigned short      mMajor;
    unsigned short      mMinor;
    unsigned short      mPatch;
    std::string mPreRelease;
    std::string mBuildMeta;
    std::string mFullVersionString;
    bool        mValidVersion;
    bool        mAnyVersion;

};

class TintMapColor
{
public:
    //std::array<rgb, 4> colors;
};

class SerializedPersonaPieceHandle
{
public:
    std::string        mPieceId;
    persona::PieceType mPieceType;
    int                mPackId;
    bool               mIsDefaultPiece;
    std::string        mProductId;
};

// auto generated forward declare list
// clang-format off
namespace Json { class ValueConstIterator; }
namespace Json { class ValueIterator; }
// clang-format on

namespace Json {

    enum class ValueType : int {
        Null = 0x0,
        Int = 0x1,
        Uint = 0x2,
        Real = 0x3,
        String = 0x4,
        Boolean = 0x5,
        Array = 0x6,
        Object = 0x7,
    };


    class Value {
    public:
        // Value inner types declare
        // clang-format off
        class CZString;
        struct CZStringCompare;
        // clang-format on

        friend class ValueIteratorBase;


        using Int = int;
        using UInt = unsigned int;
        using Int64 = int64_t;
        using UInt64 = uint64_t;
        using LargestInt = Int64;
        using LargestUInt = UInt64;
        using ArrayIndex = unsigned int;

        using iterator = ValueIterator;
        using const_iterator = ValueConstIterator;
        using ObjectValues = std::map<CZString, Value>;

        // Value inner types define
        class CZString {
        public:
            enum class DuplicationPolicy : unsigned int {
                noDuplication = 0,
                duplicate = 1,
                duplicateOnCopy = 2,
                Mask = 3,
            };
            struct StringStorage {
                DuplicationPolicy policy_ : 2;
                unsigned int      length_ : 30; // 1GB max
            };

            char const* cstr_;
            union {
                ArrayIndex    index_{};
                StringStorage storage_;
            };

            CZString(ArrayIndex index) : index_(index) {}

            CZString(char const* str, unsigned int length, DuplicationPolicy allocate) : cstr_(str) {
                storage_.policy_ = allocate;
                storage_.length_ = length & 0x3FFFFFFF;
            }
            constexpr CZString& operator=(CZString const& other) {
                if (this == &other) {
                    return *this;
                }
                cstr_ = other.cstr_;
                index_ = other.index_;
                return *this;
            }

            [[nodiscard]] constexpr std::strong_ordering operator<=>(CZString const& other) const {
                if (!cstr_) return index_ <=> other.index_;
                unsigned int this_len = this->storage_.length_;
                unsigned int other_len = other.storage_.length_;
                unsigned int min_len = std::min<unsigned int>(this_len, other_len);
                int  comp = memcmp(this->cstr_, other.cstr_, min_len);

                if (comp == 0) return this_len <=> other_len;
                return comp <=> 0;
            }

            [[nodiscard]] constexpr ArrayIndex  index() const { return index_; }
            [[nodiscard]] constexpr char const* c_str() const { return cstr_; }
            [[nodiscard]] constexpr bool        isStaticString() const {
                return storage_.policy_ == DuplicationPolicy::noDuplication;
            }
        };

        struct CZStringCompare {
        public:
            // prevent constructor by default
            CZStringCompare& operator=(CZStringCompare const&);
            CZStringCompare(CZStringCompare const&);
            CZStringCompare();
        };

        union ValueHolder {
            int64_t                      int_;    // this+0x0
            uint64_t                     uint_;   // this+0x0
            double                     real_;   // this+0x0
            bool                       bool_;   // this+0x0
            char* string_; // this+0x0
            std::map<CZString, Value>* map_;    // this+0x0
        };

        ValueHolder value_;
        struct {
            ValueType value_type_ : 8;
            bool      allocated_ : 1;
        } bits_;

    public:
        // prevent constructor by default
        Value() = default;
    };

}; // namespace Json

class SerializedSkin {
public:
    enum class TrustedSkinFlag : signed char {
        Unset = 0x0,
        False = 0x1,
        True = 0x2,
    };

    std::string                                          mId;
    std::string                                          mPlayFabId;
    std::string                                          mFullId;
    std::string                                          mResourcePatch;
    std::string                                          mDefaultGeometryName;
    mce::Image                                           mSkinImage;
    mce::Image                                           mCapeImage;
    std::vector<AnimatedImageData>                       mSkinAnimatedImages;
    Json::Value                                          mGeometryData;
    SemVersion                                           mGeometryDataEngineVersion;
    Json::Value                                          mGeometryDataMutable;
    std::string                                          mAnimationData;
    std::string                                          mCapeId;
    std::vector<SerializedPersonaPieceHandle>            mPersonaPieces;
    persona::ArmSize::Type                               mArmSizeType;
    std::unordered_map<persona::PieceType, TintMapColor> mPieceTintColors;
    UIColor                                              mSkinColor;
    TrustedSkinFlag                                      mIsTrustedSkin;
    bool                                                 mIsPremium;
    bool                                                 mIsPersona;
    bool                                                 mIsPersonaCapeOnClassicSkin;
    bool                                                 mIsPrimaryUser;
    bool                                                 mOverridesPlayerAppearance;

    static void CopyTo(SerializedSkin* from, SerializedSkin* to) {
        to->mId = from->mId;
        to->mPlayFabId = from->mPlayFabId;
        to->mFullId = from->mFullId;
        to->mResourcePatch = from->mResourcePatch;
        to->mDefaultGeometryName = from->mDefaultGeometryName;
        to->mSkinImage = from->mSkinImage;
        to->mCapeImage = from->mCapeImage;
        to->mSkinAnimatedImages = from->mSkinAnimatedImages;
        to->mGeometryData = from->mGeometryData;
        to->mGeometryDataEngineVersion = from->mGeometryDataEngineVersion;
        to->mGeometryDataMutable = from->mGeometryDataMutable;
        to->mAnimationData = from->mAnimationData;
        to->mCapeId = from->mCapeId;
        to->mPersonaPieces = from->mPersonaPieces;
        to->mArmSizeType = from->mArmSizeType;
        to->mPieceTintColors = from->mPieceTintColors;
        to->mSkinColor = from->mSkinColor;
        to->mIsTrustedSkin = from->mIsTrustedSkin;
        to->mIsPremium = from->mIsPremium;
        to->mIsPersona = from->mIsPersona;
        to->mIsPersonaCapeOnClassicSkin = from->mIsPersonaCapeOnClassicSkin;
        to->mIsPrimaryUser = from->mIsPrimaryUser;
        to->mOverridesPlayerAppearance = from->mOverridesPlayerAppearance;
    }
}; //Size: 0x268
//static_assert(sizeof(SerializedSkin) == 0x268, "SerializedSkin is not the correct size.");
