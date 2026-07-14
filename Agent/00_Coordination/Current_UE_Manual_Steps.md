# 当前 UE 编辑器人工操作清单

生成日期：2026-07-14

维护规则：本文只记录“当前这一步”需要用户在 UE 编辑器里手动完成的操作。每次进入新的人工操作阶段时，Codex 可以直接覆盖本文。

## 当前目标

排查 `BP_Hero_Phase` 中调整 `CameraSpringArm` 后，PIE 视角没有变化的问题。

## 核心判断

`AApexPlayerCharacter` 的相机链路是：

`RootComponent` -> `CameraSpringArm` -> `FollowCamera`

代码中 `CameraSpringArm` 开启了 `Use Pawn Control Rotation`：

```cpp
CameraSpringArm->bUsePawnControlRotation = true;
```

这意味着运行时 SpringArm 的旋转主要由 PlayerController 的 Control Rotation 控制。  
所以在蓝图里直接改 `CameraSpringArm` 的 Relative Rotation，通常不会改变 PIE 视角。

## 1. 先确认 PIE 真的生成了 BP_Hero_Phase

1. 打开当前测试关卡。
2. 打开 World Settings。
3. 检查 GameMode Override。
4. 打开当前 GameMode 蓝图。
5. 确认：
   - `Default Pawn Class` 是 `BP_Hero_Phase`
   - `Player Controller Class` 是 `BP_ThirdPersonPlayerController`

如果 PIE 里生成的仍然是模板角色，你调整 `BP_Hero_Phase` 的相机不会有任何变化。

验证方式：

1. PIE 开始后，在 World Outliner 里搜索当前玩家角色。
2. 选中实际生成的 Pawn。
3. Details 里确认它的类名是否是 `BP_Hero_Phase`。

## 2. 确认 FollowCamera 挂在 CameraSpringArm 上

1. 打开 `BP_Hero_Phase`。
2. 在 Components 面板检查层级。
3. 正确结构应当类似：
   - CapsuleComponent
   - Mesh
   - CameraSpringArm
     - FollowCamera

如果 `FollowCamera` 不在 `CameraSpringArm` 下面，调整 SpringArm 不会影响相机。

## 3. 哪些 SpringArm 参数应该立刻有效

在 `BP_Hero_Phase` 中选中 `CameraSpringArm`，优先调整这些：

| 参数 | 作用 | 是否应当在 PIE 有变化 |
| --- | --- | --- |
| `Target Arm Length` | 相机离角色距离 | 应该有效 |
| `Socket Offset` | 相机相对 SpringArm 末端偏移，适合做越肩视角 | 应该有效 |
| `Target Offset` | SpringArm 起点偏移，适合抬高/偏移观察点 | 应该有效 |
| `Probe Size` / `Do Collision Test` | 相机碰撞回缩 | 视场景碰撞而定 |
| `Relative Rotation` | SpringArm 自身旋转 | 当前通常不会有效 |

推荐先做一个明显测试：

- `Target Arm Length` 改成 `120`
- `Socket Offset` 改成 `X=0, Y=80, Z=40`
- 保存、编译蓝图、重新 PIE

如果这样仍完全没变化，优先怀疑 PIE 没有使用 `BP_Hero_Phase`，或相机不是 `FollowCamera`。

## 4. 为什么改 Rotation 没变化

当前 C++ 设计是第三人称自由视角：

- 鼠标输入修改 PlayerController 的 Control Rotation。
- `CameraSpringArm` 跟随 Control Rotation。
- `FollowCamera` 挂在 SpringArm 末端。

所以 `CameraSpringArm` 的 Relative Rotation 会被 Control Rotation 覆盖。

如果你只是想调默认俯仰角，有两种路线：

1. 短期：进入 PIE 后用鼠标调整视角。
2. 后续代码方案：在角色生成或 PlayerController 初始化时设置初始 Control Rotation。

当前阶段先不建议关闭 `Use Pawn Control Rotation`，否则鼠标视角链路会变得不符合第三人称自由视角目标。

## 5. 如果你想要近一点、越肩一点的视角

不要改 SpringArm Rotation，优先改：

- `Target Arm Length`
- `Socket Offset`
- `Target Offset`
- `Camera Field Of View`

建议测试值：

| 参数 | 推荐值 |
| --- | --- |
| `Target Arm Length` | `260` |
| `Socket Offset` | `X=0, Y=70, Z=35` |
| `Target Offset` | `X=0, Y=0, Z=40` |
| `FollowCamera Field Of View` | `85` |

这个会更接近第三人称英雄动作游戏的越肩观察感。

## 6. 如果你改 TargetArmLength 也没变化

按顺序检查：

1. 是否保存并编译了 `BP_Hero_Phase`。
2. PIE 是否真的生成了 `BP_Hero_Phase`。
3. 当前关卡是否有手动放置的旧 Pawn，并设置了 `Auto Possess Player = Player 0`。
4. `FollowCamera` 是否仍挂在 `CameraSpringArm` 上。
5. 是否有别的 CameraActor 被关卡或 PlayerController 自动切成 View Target。
6. PIE 时选中实际 Pawn，看运行时 `CameraSpringArm.TargetArmLength` 是否等于你改的值。

## 7. 当前结论

如果你改的是 SpringArm 的旋转，不变化是符合当前代码设计的。  
如果你改的是 `Target Arm Length`、`Socket Offset`、`Target Offset` 仍然不变化，那大概率是 PIE 没有使用你正在改的 `BP_Hero_Phase`，或者 View Target 不是这个角色的 `FollowCamera`。
