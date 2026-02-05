/*
 ******************************************************************************
 *  SmartPointer.hpp
 *
 *  Basic logic for DuinoMemory smart pointers.
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
 *    Simple and lightweight smart pointers for Arduino. SmartPointer
 *    defines basic abstract behavior like dereferencing and raw
 *    pointer access.
 *
 ******************************************************************************
 */
#pragma once

namespace DuinoMemory
{
    /**
     * Base behavior for all smart pointers for Arduino. Smart pointers
     * manage memory deallocation automatically and greatly reduce risks
     * of memory leaks.
     * @param T can be any type.
     */
    template<typename T>
    class SmartPointer
    {
    public:
        // Don't manage deallocation here, derived types shall do it.
        // _data is not destroyed in base class. Concrete types must ensure
        // proper memory deallocation according to their needs.
        virtual ~SmartPointer(void) = default;

        /**
         * @return _data member as a non mutable pointer.
         */
        T* get(void) const
        {
            return _data;
        }

        /**
         * Warning:
         *   Dereferencing a null SmartPointer (* or ->) leads to undefined behavior.
         *   Always check that the pointer is valid before dereferencing:
         *       if (ptr) { ptr->method(); }
         */
        T& operator *(void) const { return *_data; }
        T* operator ->(void) const { return _data; }

        explicit operator bool(void) const
        {
            return _data != nullptr;
        }

        friend bool operator ==(const SmartPointer<T>& a, const SmartPointer<T>& b)
        {
            return a.get() == b.get();
        }

        friend bool operator !=(const SmartPointer<T>& a, const SmartPointer<T>& b)
        {
            return a.get() != b.get();
        }

        friend bool operator ==(const SmartPointer<T>& sp, const T* p)
        {
            return sp.get() == p;
        }

        friend bool operator ==(const T* p, const SmartPointer<T>& sp)
        {
            return sp.get() == p;
        }

        friend bool operator ==(const SmartPointer<T>& sp, const T& other)
        {
            return sp != nullptr && *sp == other;
        }

        friend bool operator ==(const T& other, const SmartPointer<T>& sp)
        {
            return sp == other;
        }

        friend bool operator !=(const SmartPointer<T>& sp, const T* p)
        {
            return sp.get() != p;
        }

        friend bool operator !=(T* p, const SmartPointer<T>& sp)
        {
            return sp != p;
        }

        friend bool operator !=(const SmartPointer<T>& sp, const T& other)
        {
            return !(sp == other);
        }

        friend bool operator !=(const T& other, const SmartPointer<T>& sp)
        {
            return !(sp == other);
        }

    protected:
        /**
         * Initializes this SmartPointer as the nullptr.
         */
        SmartPointer(void) = default;

        /**
         * Initializes this SmartPointer with the provided traditional
         * pointer.
         * @param data can be nullptr.
         */
        explicit SmartPointer(T* data) : _data{ data }
        {
            // Empty body
        }

        /**
         * Changes the value of the _data member.
         * CAUTION: Concrete types must manage deallocation of
         *          _data before calling this method; potential
         *          memory leak otherwise.
         * @param new_data to assign. Can be nullptr.
         */
        void set_data(T* new_data) { _data = new_data; }

    private:
        T* _data{ };
    };
}