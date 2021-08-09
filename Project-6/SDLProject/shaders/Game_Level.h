#include "Scene.h"
class Game_Level : public Scene {
public:
    void Initialize(int lives) override;
    void ProcessInput() override;
    void Update(float deltaTime) override;
    void Render(ShaderProgram *program) override;
};
