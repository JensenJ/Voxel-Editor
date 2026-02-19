#define INPUT_ACTIONS(X)                                                                           \
    X(None)                                                                                        \
    X(Camera_MoveForward)                                                                          \
    X(Camera_MoveBackward)                                                                         \
    X(Camera_MoveLeft)                                                                             \
    X(Camera_MoveRight)                                                                            \
    X(Camera_MoveUp)                                                                               \
    X(Camera_MoveDown)                                                                             \
    X(Camera_FocusViewport)                                                                        \
    X(Camera_ModifySpeed)                                                                          \
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