BassPro
=======

Fishing game for the GBA, in mode 3

Instructions for how to play are within the game. There's also an instructions image that has everything.

This uses Affine Sprites, and sprites in general, which were way above and beyond the scope of the project. We were supposed to make the game in Mode 3 where we had to keep track of pixels we changed so that we could clean up, but using hardware sprites dodged that ugliness, and gave much better performance. Not that the performance in Mode 3 is really that great anyways. I would have loved to have used the HBlank interrupt here for the waves, but I couldn't figure out how to write the handler in time for the project due date.
