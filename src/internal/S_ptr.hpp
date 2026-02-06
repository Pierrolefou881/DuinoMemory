/*
 ******************************************************************************
 *  S_ptr.hpp
 *
 *  Shared pointer for Arduino.
 *
 *  Author: Pierre DEBAS
 *  Copyright (c) 2026
 *
 *  MIT License
 *  https://github.com/Pierrolefou881/DuinoMemory
 *
 *  SPDX-License-Identifier: MIT
 *
 *  Description:
 *    Lightweight shared pointer similar to the STL std::shared_ptr.
 *    Uses a control block and reference count to manage memory deallocation.
 *    When the reference count reaches zero, the object gets destroyed.
 *
 ******************************************************************************
 */
#pragma once
#include "SmartPointer.hpp"
#include <stddef.h>

namespace DuinoMemory
{
    /**
     * Pointer wrapper that automatically deallocates memory when
     * reference count to the pointed object drops to 0. This means
     * that several client objects can point to the same data.
     * @param T can be any type.
     */
    template<typename T>
    class S_ptr final : public SmartPointer<T>
    {
    public:
        /**
         * Initializes this S_ptr as nullptr.
         */
        S_ptr(void) = default;

        /**
         * Initializes this S_ptr with the provided pointer to data. If data not null,
         * increases the reference count to 1.
         * @param data pointer. Can be nullptr.
         */
        explicit S_ptr(T* data) : SmartPointer<T>{ data }
        {
            if (data != nullptr)
            {
                _ref_count = new size_t{ 1 };
            }
        }

        S_ptr(const S_ptr<T>& other) : SmartPointer<T>{ other.get() }, _ref_count{ other._ref_count }
        {
            if (other.get() != nullptr)
            {
                (*_ref_count)++;
            }
        }

        ~S_ptr(void)
        {
            release();
        }

        /**
         * @return the number of active references to this S_ptr.
         */
        size_t count(void) const noexcept
        {
            return _ref_count != nullptr ? *_ref_count : 0;
        }

        /**
         * CAUTION: Do not copy raw pointer from another S_ptr. Directly copy the S_ptr instead.
         *          You can also use this operator with functions that return raw pointers, like factories.
         * EXAMPLE: ptr = other.get();                  ==> DANGER ! Undefined behavior.
         *          ptr = other;                        ==> SAFE, very common use case.
         *          ptr = some_object->build_object();  ==> SAFE, common use case.
         */
        S_ptr<T>& operator =(T* data_ptr)
        {
            if (data_ptr != SmartPointer<T>::get())
            {
                release();
                SmartPointer<T>::set_data(data_ptr);
                _ref_count = data_ptr != nullptr ? new size_t{ 1 } : nullptr;
            }
            return *this;
        }

        S_ptr<T>& operator =(const S_ptr<T>& other)
        {
            if (this != &other)
            {
                release();
                auto other_data = other.get();
                SmartPointer<T>::set_data(other_data);
                _ref_count = other._ref_count;
                if (other_data != nullptr)
                {
                    (*_ref_count)++;
                }
            }
            return *this;
        }

    private:
        size_t* _ref_count{ };

        void release(void)
        {
            auto data = SmartPointer<T>::get();
            if (data != nullptr)
            {                
                (*_ref_count)--;
                
                if (*_ref_count == 0)
                {
                    delete data;
                    delete _ref_count;
                }
            }

            SmartPointer<T>::set_data(nullptr);
            _ref_count = nullptr;
        }
    };

    /**
     * @return a S_ptr pointing to a default instance of T.
     */
    template<typename T>
    S_ptr<T> make_shared(void)
    {
        return S_ptr<T>{ new T{ } };
    }

    /**
     * Creates a S_ptr pointing to an instance of T created with
     * provided parameters.
     * @param args must match any parameterized constructor of T.
     * @return a S_ptr instance pointing to the newly created instance of T.
     */
    template<typename T, class... Args>
    S_ptr<T> make_shared(Args&&... args)
    {
        return S_ptr<T>{ new T(args...) };
    }

    /**
     * Creates a new instance of S_ptr<t> holding a default initialized
     * instance of U.
     * @param T can be any type.
     * @param U is a derived type of T.
     * @return a new S_ptr<T> wrapping the newly instanced U.
     */
    template<typename T, typename U>
    S_ptr<T> make_shared(void)
    {
        return S_ptr<T>{ new U{ } };
    }

    /**
     * Creates a new instance of S_ptr<t> holding a instance of U initialized
     * with given parameters.
     * @param T can be any type.
     * @param U is a derived type of T.
     * @param Args types of arguments.
     * @param args must match one of U's parameterized constructors.
     * @return a new S_ptr<T> wrapping the newly instanced U.
     */
    template<typename T, typename U, class... Args>
    S_ptr<T> make_shared(Args&&... args)
    {
        return S_ptr<T>{ new U(args...) };
    }
}