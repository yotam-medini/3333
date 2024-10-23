#include "ui.h"
#include <memory>

class UIImpl {
 public:
  UIImpl() {
  }
};

UI::UI() {
  impl_ = std::make_unique<UIImpl>();
}

UI::~UI() {
}

int UI::run() {
  int rc = 0;
  return rc;
}
