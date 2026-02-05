/*
 ******************************************************************************
 *  DuinoMemory.hpp
 *
 *  Simple and lightweight smart pointers for the Arduino environment.
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
 *    Small memory allocation library that manages smart pointers to avoid
 *    using new and delete operators. Smart pointers automatically manage
 *    memory allocation and deallocation.
 *
 ******************************************************************************
 */
#pragma once
#include "internal/U_ptr.hpp"
#include "internal/S_ptr.hpp"