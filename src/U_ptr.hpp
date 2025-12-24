/*
 * ----------------------------------------------------------------------------
 * U_ptr
 * Custom implementation of unique pointer for Arduino.
 * Part of the ArduinoLibraries project, to be used with any Arduino board.
 * <https://github.com/Pierrolefou881/ArduinoLibraries>
 * ----------------------------------------------------------------------------
 *
 * Copyright (C) 2025  Pierre DEBAS
 * <dpierre394@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once
#include "SmartPointer.hpp"

namespace Memory
{
    /**
     * Pointer wrapper that automatically deallocates memory
     * when destroyed. Copy means change of ownership of
     * the _data member, i.e. the copied member becomes
     * nullptr while the copy holds exclusively the data.
     * @param T can be of any type.
     */
    template<typename T>
    class U_ptr : public SmartPointer<T>
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
        U_ptr(T* data) : SmartPointer<T>{ data }
        {
            // Empty body.
        }

        U_ptr(const U_ptr<T>& other) : SmartPointer<T>{ other.get() }
        {
            ((U_ptr<T>&) other).set_data(nullptr);
        }

        U_ptr(U_ptr<T>&& other) noexcept : SmartPointer<T>{ other.get() }
        {
            other.set_data(nullptr);
        }

        virtual ~U_ptr(void)
        {
            delete SmartPointer<T>::get();
        }

        U_ptr<T>& operator =(T* data_ptr)
        {
            auto tmp = SmartPointer<T>::get();
            SmartPointer<T>::set_data(data_ptr);
            delete tmp;
            return *this;
        }

        U_ptr<T>& operator =(const U_ptr<T>& other)
        {
            change_owner(other, *this);
            return *this;
        }

        U_ptr<T>& operator =(U_ptr<T>&& other) noexcept
        {
            change_owner(other, *this);
            return *this;
        }

    private:
        static void change_owner(const U_ptr<T>& source, const U_ptr<T>& destination)
        {   
            // Discard const qualifiers
            auto del = ((U_ptr<T>&) destination).get();
            ((U_ptr<T>&) destination).set_data(((U_ptr<T>&) source).get());
            ((U_ptr<T>) source).set_data(nullptr);
            delete del;
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
        return { new T{ } };
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
        return { new T{ args... } };
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
        return { new U{ } };
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
        return { new U{ args... } };
    }
}