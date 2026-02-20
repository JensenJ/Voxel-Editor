#define INPUT_ACTIONS(X)                                                                           \
    X(None)                                                                                        \
    X(FreeCam_MoveForward)                                                                         \
    X(FreeCam_MoveBackward)                                                                        \
    X(FreeCam_MoveLeft)                                                                            \
    X(FreeCam_MoveRight)                                                                           \
    X(FreeCam_MoveUp)                                                                              \
    X(FreeCam_MoveDown)                                                                            \
    X(FreeCam_FocusViewport)                                                                       \
    X(FreeCam_IncreaseSpeed)                                                                       \
    X(FreeCam_DecreaseSpeed)                                                                       \
    X(FreeCam_ZoomIn)                                                                              \
    X(FreeCam_ZoomOut)                                                                             \
    X(Debug_Exit)                                                                                  \
    X(Debug_Wireframe)

enum class InputAction {
#define X(name) name,
    INPUT_ACTIONS(X)
#undef X
};

static inline std::string ActionToString(InputAction action) {
    switch (action) {
#define X(name)                                                                                    \
    case InputAction::name:                                                                        \
        return #name;
        INPUT_ACTIONS(X)
#undef X
    }
    return "None";
}

static inline InputAction StringToAction(const std::string& str) {
#define X(name)                                                                                    \
    if (str == #name)                                                                              \
        return InputAction::name;
    INPUT_ACTIONS(X)
#undef X
    return InputAction::None;
}