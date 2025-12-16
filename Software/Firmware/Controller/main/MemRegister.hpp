// MemRegister.hpp

// internal memory register

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

#pragma once

#include <Arduino.h>
#include <ratpak.h>

class MemRegister
{
public:
  MemRegister()
  {
    _reg = nullptr;
  }

  virtual ~MemRegister()
  {
  }

  void set(PRAT p)
  {
    if (!zerrat(p))
    {
      DUPRAT(_reg, p);
    }
    else
    {
      destroyrat(_reg);
    }
  }

  PRAT get() const
  {
    if (_reg != nullptr)
    {
      return (_reg);
    }
    else
    {
      return (rat_zero);
    }
  }

  void clear()
  {
    destroyrat(_reg);
  }

private:
  PRAT _reg;
};
