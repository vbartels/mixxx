#ifndef UTIL_PARENTED_PTR_H
#define UTIL_PARENTED_PTR_H

#include <QPointer>
#include "util/assert.h"

/**
 * Use this wrapper class to clearly represent a raw pointer that is owned by the QT object tree.
 * Objects which both derive from QObject AND have a parent object, have their lifetime governed by the QT object tree, 
 * and thus such pointers do not require a manual delete to free the heap memory when they go out of scope.
**/
template <typename T>
class parented_ptr {
  public:
    explicit parented_ptr(T* t) : m_pObject(t) {
        DEBUG_ASSERT(t->parent() != nullptr);
    }

    /* If U* is convertible to T* then we also want parented_ptr<U> convertible to parented_ptr<T> */
    template <typename U>
    parented_ptr(parented_ptr<U>&& u, typename std::enable_if<std::is_convertible<U*, T*>::value, void>::type * = 0)
            : m_pObject(u.get()) {
        DEBUG_ASSERT(u->parent() != nullptr);
    }

    // Delete copy constructor and copy assignment operator
    parented_ptr(const parented_ptr<T>&) = delete;
    parented_ptr& operator=(const parented_ptr<T>&) = delete;

    parented_ptr() : m_pObject(nullptr) {}

    ~parented_ptr() = default;

    T* get() const {
        return m_pObject;
    }

    T& operator* () const {
        return *m_pObject;
    }

    T* operator-> () const {
        return m_pObject;
    }

    bool operator== (const parented_ptr& other) const {
        return m_pObject == other.m_pObject;
    }

    bool operator== (const T* other) const {
        return m_pObject == other;
    }

    bool operator!= (const parented_ptr& other) const {
        return m_pObject != other.m_pObject;
    }

    bool operator!= (const T* other) const {
        return m_pObject != other;
    }

    operator bool() const {
        return m_pObject != nullptr;
    }

    /*
     * If U* is convertible to T* then we also want parented_ptr<U> assignable to parented_ptr<T>
     * E.g. parented_ptr<Base> base = make_parented<Derived>(); should work as expected.
     */
    template <typename U>
    typename std::enable_if<std::is_convertible<U*, T*>::value, parented_ptr<T>&>::type
            operator=(parented_ptr<U>&& p) {
        m_pObject = p.get();
        return *this;
    }

    QPointer<T> toWeakRef() {
        return m_pObject;
    }

  private:
    T* m_pObject;
};

namespace {

template<typename T, typename... Args>
inline parented_ptr<T> make_parented(Args&&... args) {
    return parented_ptr<T>(new T(std::forward<Args>(args)...));
}

} // namespace

#endif // UTIL_PARENTED_PTR_H
