/*
 * ----------------------------------------------------------------------------
 * S_ptr
 * Custom implementation of shared pointer for Arduino.
 * <https://github.com/Pierrolefou881/DuinoMemory>
 * ----------------------------------------------------------------------------
 *
 * Copyright (C) 2026  Pierre DEBAS
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
#include <stdint.h>

namespace Memory
{
    /**
     * Pointer wrapper that automatically deallocates memory when
     * reference count to the pointed object drops to 0. This means
     * that several client objects can point to the same data.
     * @param T can be any type.
     */
    template<typename T>
    class S_ptr : public SmartPointer<T>
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
        S_ptr(T* data) : SmartPointer<T>{ data }, _ref_count{ new uint16_t{ 0 } }
        {
            if (data != nullptr)
            {
                (*_ref_count)++;
            }
        }

        S_ptr(const S_ptr<T>& other) : SmartPointer<T>{ other.get() }, _ref_count{ other._ref_count }
        {
            if (other.get() != nullptr)
            {
                (*_ref_count)++;
            }
        }

        S_ptr(S_ptr&& other) noexcept : SmartPointer<T>{ other.get() }, _ref_count{ other._ref_count }
        {
            if (other.get() != nullptr)
            {
                (*_ref_count)++;
            }
        }

        virtual ~S_ptr(void)
        {
            decrease_ref_count();
        }

        /**
         * @return the number of active references to this S_ptr.
         */
        uint16_t count(void) const
        {
            return *_ref_count;
        }

        S_ptr<T>& operator =(T* data_ptr)
        {
            if (data_ptr != *this)
            {
                decrease_ref_count();
                SmartPointer<T>::set_data(data_ptr);
                _ref_count = new uint16_t{ };
                if (data_ptr != nullptr)
                {
                    *_ref_count += 1;
                }
            }
            return *this;
        }

        S_ptr<T>& operator =(const S_ptr<T>& other)
        {
            if (other != *this)
            {
                decrease_ref_count();
                SmartPointer<T>::set_data(other.get());
                _ref_count = other._ref_count;
                if (other != nullptr)
                {
                    *_ref_count += 1;
                }
            }
            return *this;
        }

        S_ptr<T>& operator =(S_ptr<T>&& other) noexcept
        {
            if (other != *this)
            {
                decrease_ref_count();
                SmartPointer<T>::set_data(other.get());
                _ref_count = other._ref_count;
                if (other != nullptr)
                {
                    *_ref_count += 1;
                }
            }
            return *this;
        }

    private:
        uint16_t* _ref_count{ new uint16_t{ 0 }};

        void decrease_ref_count(void)
        {
            if (*this == nullptr)
            {
                return;
            }

            *_ref_count -= 1;
            if (*_ref_count == 0)
            {
                delete _ref_count;
                auto tmp = SmartPointer<T>::get();
                SmartPointer<T>::set_data(nullptr);
                delete tmp;
            }
        }
    };

    /**
     * @return a S_ptr pointing to a default instance of T.
     */
    template<typename T>
    S_ptr<T> make_shared(void)
    {
        return { new T{ } };
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
        return { new T{ args... } };
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
        return { new U{ } };
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
        return { new U{ args... } };
    }
}