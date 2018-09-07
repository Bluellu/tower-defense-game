# tower-defense-game
A basic Tower Defense project built in Unreal Engine 4 with a combination of C++ and Blueprints. 
Based on the C++ FPS template.

**Gameplay demo video:**

https://www.youtube.com/watch?v=HTsvBcI-0qY&feature=youtu.be

---------------------------------------------------------------------------------------------------------------------------
### GAMEPLAY INSTRUCTIONS 

### Keyboard:

On shooting mode (default): 
  - **Left mouse button** - Shoot
  
On tower-building mode (hold **Left Shift**):
  - **E** - Switch defense tower 
  - **Left mouse button** - Place Defense Tower
  


### Controller:

On shooting mode (default): 
  - **Right Trigger** - Shoot

On tower-building mode (hold **left bumper**):
  - **Right Bumper** - Switch Defense Tower
  - **Right Trigger** - Place Defense Tower
---------------------------------------------------------------------------------------------------------------------------
### DEVELOPMENT NOTES 

For coding style, I followed the standards from the base project.

The bulk of this project is coded in C++,  with  classes and functions designed to be exposed to Blueprints for use by designers (e.g: the SpawnWave function in the EnemySpawner class, which allows the scheduling of enemy waves with a custom number of enemies of a chosen type).

Blueprint logic was used when convenient, in particular when Blueprint classes needed to be accessed directly. All dynamic material changes were done through Blueprint logic (e.g: changes to indicate deployability and status of a defense tower, and the temporary change of color of enemies when they take damage). 

Projectile shots were replaced with hitscan shooting, using limited-range line tracing (I left debug lines visible as placeholders for a particle effect).

Two types of defense towers are available, a Turret type which targets and damages enemies in range (DefenseTower), and a sticky type which slows enemy movement down while they overlap with it.

For enemy behavior, I designed a behavior tree (EnemyBT2) which creates a dynamic path towards the goal based on semi-randomized waypoint selection (enemies choose random waypoints within a given cone of view that always points towards the goal, generating dynamic sideways movement). If no waypoint is present, enemies move directly towards the goal. Variations of enemy movement can be created by rearranging the waypoints and placing them at higher or smaller concentrations (along with changing cone angle and speed settings of different types of enemy). Waypoints can also be arranged into trails to direct enemy movement towards specific paths.

Multiple EnemySpawners can also be placed on the level and timed with different waves of enemies, which will all move towards a single placed goal. The victory state is called from a Spawner Blueprint after it releases the last wave.

-----------------------------------------------------------------------------------------------------------------------
**NOTE:** Sometimes on opening the project, an editor bug occurs which resets the Blueprint children of AEnemy (EnemyCharacter_BP and TankEnemy_BP) to their default class values. This removes the Behavior Tree (EnemyBT2), making it so spawned enemies do not move, and it also resets their differing values. Normally, it only happens if the parent C++ class is edited, but sometimes it appears to be random.
Just in case, here follow the values that these blueprints should have for correct behavior:

**EnemyCharacter_BP** (Content/FirstPersonCPP/Blueprints)
- HP: 10
- Resource Points: 5
- Enemy Behavior: EnemyBT2
    
**TankEnemy_BP** (Content/FirstPersonCPP/Blueprints)
- HP: 50
- Resource Points: 15
- Enemy Behavior: EnemyBT2
- Cone Half Angle: 50
    
In the case this bug occurs, changing these settings should restore the correct behavior.
(This also applies to certain animations and sounds).
