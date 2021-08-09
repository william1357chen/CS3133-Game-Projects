#include "Scene.h"
class Game_Over : public Scene {
public:
    void Initialize(int lives) override;
    void ProcessInput() override;
    void Update(float deltaTime) override;
    void Render(ShaderProgram *program) override;
};
