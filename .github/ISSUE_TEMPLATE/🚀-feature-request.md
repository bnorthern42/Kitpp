---
name: "\U0001F680 Feature Request"
about: Suggest an idea for the library (e.g., new Math functions, new modules)
title: ''
labels: ''
assignees: ''

---

title: "[Feature]: "
labels: ["enhancement"]
body:
  - type: markdown
    attributes:
      value: |
        Thanks for suggesting a new feature for kitpp!
  - type: textarea
    id: problem
    attributes:
      label: Is your feature request related to a problem?
      description: A clear and concise description of what the problem is.
      placeholder: "I'm always frustrated when I have to manually implement dot product for std::vector..."
    validations:
      required: true
  - type: textarea
    id: solution
    attributes:
      label: Describe the solution you'd like
      description: Describe the API you envision. Mock code is very helpful here!
      render: cpp
      placeholder: |
        // I would like to be able to do this:
        std::vector<double> a = {1, 2, 3};
        std::vector<double> b = {4, 5, 6};
        double result = kitpp::math::dot(a, b);
    validations:
      required: true
  - type: textarea
    id: alternatives
    attributes:
      label: Describe alternatives you've considered
      description: A clear and concise description of any alternative solutions or features you've considered.
  - type: textarea
    id: context
    attributes:
      label: Additional context
      description: Add any other context or screenshots about the feature request here.
