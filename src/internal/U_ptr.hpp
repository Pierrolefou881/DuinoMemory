/*
 ******************************************************************************
 *  U_ptr.hpp
 *
 *  Simple and lightweight unique pointer for Arduino.
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
 *    Like std::unique_ptr, U_ptr changes ownership on new assignment.
 *    The object pointed to is destroyed whenever the U_ptr goes out of
 *    scope. U_ptr does not allow copying.
 *
 ******************************************************************************
 */
#pragma once
#include "SmartPointer.hpp"

namespace DuinoMemory
{
    /**
     * Pointer wrapper that automatically deallocates memory
     * when destroyed. U_ptr does not allow copying; ownership is transferred
     * on each assignment.
     * @param T can be of any type. When using U_ptr<Base> with derived objects, 
     *          Base must have a virtual destructor.
     */
    template<typename T>
    class U_ptr final : public SmartPointer<T>
    {
    public:
        /**
         * Initializes this U_ptr as the nullptr.
         */
        U_ptr(void) = default;

        /**
         * Initializes this U_ptr with the provided data pointer.
         * @param data can be nullptr.
         */
        explicit U_ptr(T* data) : SmartPointer<T>{ data }
        {
            // Empty body.
        }

        U_ptr(const U_ptr<T>& other) = delete;

        U_ptr(U_ptr<T>&& other) noexcept : SmartPointer<T>{ other.get() }
        {
            other.set_data(nullptr);
        }

        // Automatically destroys data when out of scope.
        ~U_ptr(void)
        {
            delete SmartPointer<T>::get();
        }

        U_ptr<T>& operator =(T* data_ptr)
        {
            auto tmp = SmartPointer<T>::get();
            // Avoid self assignment.
            if (data_ptr != tmp)
            {
                SmartPointer<T>::set_data(data_ptr);
                delete tmp;
            }

            return *this;
        }

        U_ptr<T>& operator =(const U_ptr<T>& other) = delete;

        U_ptr<T>& operator =(U_ptr<T>&& other) noexcept
        {
            // Avoid self assignment.
            if (this == &other)
            {
                return *this;
            }

            delete SmartPointer<T>::get();
            SmartPointer<T>::set_data(other.get());
            other.set_data(nullptr);
            return *this;
        }

        T* release(void)
        {
            auto ptr = SmartPointer<T>::get();
            SmartPointer<T>::set_data(nullptr);
            return ptr;
        }
    };

    /**
     * Create a new instance of U_ptr<T> holding a default T.
     * @param T can be any type.
     * @return a new instance of U_ptr<T> holding a default initialized
     *         instance of T.
     */
    template<typename T>
    U_ptr<T> make_unique(void)
    {
        return U_ptr<T>{ new T{ } };
    }

    /**
     * Creates a new instance of U_ptr<T> using the template type's
     * parametrized constructor.
     * @param T can be any type.
     * @param Args types of arguments.
     * @param args must match one of T's parametrized constructors.
     * @return a new instance of U_ptr<T> wrapping the instanced object.
     */
    template<typename T, class... Args>
    U_ptr<T> make_unique(Args&&... args)
    {
        return U_ptr<T>{ new T(args...) };
    }

    /**
     * Creates a new instance of U_ptr<t> holding a default initialized
     * instance of U.
     * @param T can be any type.
     * @param U is a derived type of T.
     * @return a new U_ptr<T> wrapping the newly instanced U.
     */
    template<typename T, typename U>
    U_ptr<T> make_unique(void)
    {
        return U_ptr<T>{ new U{ } };
    }

    /**
     * Creates a new instance of U_ptr<t> holding a instance of U initialized
     * with given parameters.
     * @param T can be any type.
     * @param U is a derived type of T.
     * @param Args types of arguments.
     * @param args must match one of U's parameterized constructors.
     * @return a new U_ptr<T> wrapping the newly instanced U.
     */
    template<typename T, typename U, class... Args>
    U_ptr<T> make_unique(Args&&... args)
    {
        return U_ptr<T>{ new U(args...) };
    }
}