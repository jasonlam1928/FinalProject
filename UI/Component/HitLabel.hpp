#include <allegro5/allegro_font.h>
#include <allegro5/color.h>
#include <memory>
#include <string>

#include "Engine/IObject.hpp"

class PlayScene;

namespace Engine {
    /// <summary>
    /// A simple static text object.
    /// </summary>
    class HitLabel : public IObject {
    protected:
        // Smart pointer to font.
        std::shared_ptr<ALLEGRO_FONT> font;
        int Timer=0;
        PlayScene *getPlayScene();

    public:
        // Text to draw.
        std::string Text;
        // Text's color.
        ALLEGRO_COLOR Color;
        /// <summary>
        /// Construct a label object.
        /// </summary>
        /// <param name="text">Text to draw.</param>
        /// <param name="font">Font to draw in.</param>
        /// <param name="fontSize">Text's font size.</param>
        /// <param name="x">X-coordinate.</param>
        /// <param name="y">Y-coordinate.</param>
        /// <param name="r">Color's red value.</param>
        /// <param name="g">Color's green value.</param>
        /// <param name="b">Color's blue value.</param>
        /// <param name="a">Color's alpha value.</param>
        /// <param name="anchorX">The centerX of the object. (0, 0) means top-left, while (1, 0) means top-right.</param>
        /// <param name="anchorY">The centerY of the object. (0, 1) means bottom-left, while (1, 1) means bottom-right.</param>
        explicit HitLabel(const std::string &text, const std::string &font, float x, float y, float anchorX = 0, float anchorY = 0);
        /// <summary>
        /// Draw text with loaded font.
        /// </summary>
        void Draw() const override;

        void Update(float deltaTime) override;
        /// <summary>
        /// Return text width.
        /// </summary>
        /// <returns>Width of the text.</returns>
        int GetTextWidth() const;
        /// <summary>
        /// Return text height.
        /// </summary>
        /// <returns>Height of the text.</returns>
        int GetTextHeight() const;
    };
}

