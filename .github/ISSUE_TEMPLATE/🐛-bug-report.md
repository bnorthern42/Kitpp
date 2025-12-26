---
name: "\U0001F41B Bug Report"
about: Create a report to help us improve kitpp
title: ''
labels: ''
assignees: ''

---

title: "[Bug]: "
labels: ["bug", "triage"]
body:
  - type: markdown
    attributes:
      value: |
        Thanks for taking the time to fill out this bug report!
  - type: textarea
    id: what-happened
    attributes:
      label: What happened?
      description: Also tell us, what did you expect to happen?
      placeholder: "I tried using kitpp::log::info inside an OpenMP loop and..."
    validations:
      required: true
  - type: textarea
    id: repro
    attributes:
      label: Reproduction Steps / Code Snippet
      description: Please provide a minimal code snippet that reproduces the issue.
      render: cpp
      placeholder: |
        #include <kitpp/kitpp.hpp>

        int main() {
            // Your code here
            return 0;
        }
    validations:
      required: true
  - type: dropdown
    id: build-system
    attributes:
      label: Build System
      description: Which build system were you using?
      options:
        - Meson
        - Makefile
        - CMake (Custom/Legacy)
        - Other
    validations:
      required: true
  - type: input
    id: compiler
    attributes:
      label: Compiler & Version
      description: e.g. GCC 13.2, Clang 16, MSVC 2022
      placeholder: g++ --version
    validations:
      required: true
  - type: dropdown
    id: os
    attributes:
      label: Operating System
      options:
        - Linux
        - macOS
        - Windows
        - Other
    validations:
      required: true
  - type: textarea
    id: logs
    attributes:
      label: Relevant Log Output
      description: Please copy and paste any relevant build errors or crash logs. This will be automatically formatted into code, so no need for backticks.
      render: shell-session
