# Fliproll, take two

**House rules**
- Plain structs, plain functions, one main loop, **procedural**.
- No overengineering architecture until something works on-screen.
- Only abstract after the same thing has been copy-pasted three times.
- If stuck for more than 20 minutes make step smaller.

## Prep
- [X] Prepare project directory
- [X] Set up development packages
- [X] Write a Makefile
- [X] Open window that closes on Esc
- [X] Clear background

## Canvas
- [ ] Draw circle that follows the mouse
- [ ] Paint dots that stay while mouse is held: Draw into an offscreen texture (RenderTexture or SDL_Texture target), then draw that texture every frame.
- [ ] Connect dots with lines to close the gaps
- [ ] Change brush size with scroll wheel
- [ ] Press C to clear the canvas

**Reminder that all of this so far has to remain one loop**

## Fancy IMMEDIATE MODE UI
- [ ] Write a Button function and draw one THROUGH THE SIMPLE MAIN LOOP THAT SHOULD STILL EXIST AND NOT SOMETHING MORE OVERENGINEERED, OKAY?
- [ ] Change its color when hovered
- [ ] Add hover_t and lerp it for fancy animations!
- [ ] Consider making a button struct maybe maybe
- [ ] Add a press squish with press_t
- [ ] Implement a sidebar panel STILL THROUGH A SIMPLE MAIN LOOP OKAY?