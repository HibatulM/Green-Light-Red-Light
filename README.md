# Green-Light-Red-Light

A distributed systems project simulating the classic *Red Light, Green Light* game using multiple communicating Arduino microcontrollers.

---

## 📖 Overview

This project models the familiar playground game with one **simulated caller** (controls red/green light) and one **simulated player** (runs toward the goal). All light coordination and player movement data is recorded by a dedicated **scoreboard Arduino**.

> In the real-world game, a caller monitors players and disqualifies anyone who moves while the light is red. Our system replicates this mechanic electronically.

---

## 🔧 Hardware Components

| Component | Role |
|---|---|
| **Light Arduino** | Drives a pair of red/green LEDs representing the stoplight |
| **Player Arduino** | Reads pushbutton input to simulate player steps |
| **Scoreboard Arduino** | 16x2 LCD display showing score and game status |
| **Central Controller Arduino** | Coordinates communication between all nodes |

---

## 🎮 How It Works

1. The **central controller** determines the light timing schedule and broadcasts state changes.
2. A **green light** allows the player to press the button and accumulate steps (score).
3. A **red light** freezes play — any step input during red immediately **ends the game**.
4. The **LCD scoreboard** updates in real time to reflect the player's current score and game status.

---

## ✨ Key Features

- **Distributed Architecture** — Multiple microcontrollers collaborate over a shared communication bus to perform a unified task.
- **Adaptive Timing** — The system responds dynamically to input events such as player movements and light state changes.
- **Real-Time Feedback** — The 16x2 LCD provides live game status and score tracking.
- **Penalty Detection** — Automatic game-over logic triggers when a step is detected on a red light.

---

## 🗺️ System Architecture
