#ifndef __NonCopyable_H__
#define __NonCopyable_H__

class NonCopyable
{
public:
    NonCopyable() = default;
    ~NonCopyable() = default;
    NonCopyable(const NonCopyable &) = delete;
    auto operator=(const NonCopyable &) -> NonCopyable & = delete;
    NonCopyable(const NonCopyable &&) = delete;
    auto operator=(const NonCopyable &&) -> NonCopyable & = delete;
};

#endif
