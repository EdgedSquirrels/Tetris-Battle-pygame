# coding: utf-8
import pygame


def main():
    # Settings
    width = 800
    height = 600
    color_bg = (0, 0, 0)
    color_text = (200, 200, 200)
    button_clicked = False
    running = True

    # Init
    pygame.init()
    screen = pygame.display.set_mode((width, height))
    pygame.display.set_caption("Rect Demo")

    # Text
    font = pygame.font.SysFont("Arial", 35)
    text = font.render("Button", True, color_text)
    text_clicked = font.render("Clicked", True, color_text)
    text_rect = text_clicked.get_rect(center=(width / 2, height / 2))

    # Run
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

            if event.type == pygame.MOUSEBUTTONDOWN:
                button_clicked = True if text_rect.collidepoint(event.pos) else False

        # Screen
        screen.fill(color_bg)

        # Draw
        pygame.draw.rect(screen, (100, 100, 100), text_rect)

        if button_clicked:
            screen.blit(text_clicked, text_rect)
        else:
            screen.blit(text, text_rect)

        # Updates
        pygame.display.update()


if __name__ == "__main__":
    main()
