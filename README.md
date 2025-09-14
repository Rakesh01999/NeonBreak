
## 🕹️ NeonBreak-OpenGL

**NeonBreak-OpenGL** is a modern recreation of the classic arcade game *NeonBreak*, built using C++ and OpenGL. It captures the essence of the original brick-breaking gameplay with smooth graphics, paddle control via mouse, and a clean retro aesthetic.

---

## 📜 Game History (Short Version)

Originally released by **Taito in 1986**, *NeonBreak* expanded on Atari’s *Breakout* by introducing power-ups, layered visuals, and diverse brick layouts. Its popularity led to sequels like *Revenge of Doh* and inspired countless clones across home computers and consoles.

---

## 🎮 Gameplay Overview

- Control the paddle using your **mouse**
- Break all the bricks to clear the level
- Watch out for ball speed and angles
- Restart the game anytime by pressing **R**

---

## 🧱 Features

- Smooth ball physics and paddle collision
- Dynamic brick layout with scoring
- Game Over and Restart logic
- Credits and menu screens with modern gradients
- Built entirely with **OpenGL (FreeGLUT)** and **C++**

---

## 🛠️ How to Run

### Requirements:
- Windows with MSYS2 or MinGW
- FreeGLUT installed (`mingw-w64-x86_64-freeglut`)
- Visual C++ Redistributable (for runtime DLLs)

### Compile:
```bash
# g++ NeonBreak.cpp -o NeonBreak -lfreeglut -lopengl32 -lglu32
g++ -o NeonBreak NeonBreak.cpp -lfreeglut -lopengl32 -lglu32
```

### Run:
```bash
./NeonBreak.exe
```

---

## 👨‍💻 Credits

Developed by:
- **Rakesh Biswas**
- **Nafis Ahamed**

---
