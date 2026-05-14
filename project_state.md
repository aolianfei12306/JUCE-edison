# Project: Open Edison

> 轻量化桌面音频编辑器，类似 FL Studio Edison，聚焦波形编辑、选区操作与便捷导出。

---

## 终极目标

构建一个基于 JUCE 8 的轻量桌面音频编辑器，覆盖 FL Studio Edison 的**核心波形编辑功能**，并实现比 Edison 更便捷的**选区拖拽导出 WAV**。

## 当前状态

| 项目 | 值 |
|------|-----|
| **阶段** | P4+++: 三轮审计修复（Undo 未清空 Bug 修复，Ctrl+O 键冲突解决，整体功能审计） |
| **完成度** | ~99% (P0: 100%, P1: 100%, P2: 100%, P3: 100%, P4: 100%, P4+ 审计修复: ✅, P4++ 审计修复: ✅) |
| **LOC** | ~4,130 行（31 个源文件） |
| **最后更新** | 2026-05-14 13:16 CST |
| **技术栈** | JUCE 8 + C++20 + CMake |
| **目标平台** | Windows（优先）/ Linux |

## 进度记录

| 日期 | 阶段 | 完成内容 | 状态 |
|------|------|---------|------|
| 2026-05-02 | P0 构建 | JUCE 8 构建系统 + 基础编辑器骨架（14 个源文件，编译通过） | ✅ |
| 2026-05-02 | P1 频谱 | 频谱/声谱图视图（STFT 渲染、波形/频谱切换、Ctrl+S 快捷键） | ✅ |
| 2026-05-02 | P1 处理 | Undo/Redo 框架（AudioModifyAction）+ Silence Selection（Ctrl+D），绑定 Ctrl+Z / Ctrl+Shift+Z | ✅ |
| 2026-05-02 | P1 处理 | Reverse / Normalize / Fade In / Fade Out 选区处理（Ctrl+R/Ctrl+N/Ctrl+I/Ctrl+O）| ✅ |
| 2026-05-02 | P2 快捷键 | Delete/Backspace 静音 + 确认 Space 播放/暂停快捷键已绑定 | ✅ |
| 2026-05-02 | P2 滚轮 | 鼠标滚轮精细缩放——Ctrl+滚轮缩放（因子1.05），滚轮水平滚动，Waveform/Spectrogram 双视图 | ✅ |
| 2026-05-02 | P0 导出 | DragExport 集成——Alt+选区内拖动导出 WAV（修复 44100 硬编码采样率 Bug），构建通过 | ✅ |
| 2026-05-02 | P2 标记 | Marker/Cue Points——M键添加，Shift+M移除，[/]导航，青色标记旗标+点击跳转 | ✅ |
| 2026-05-02 | P2 Zoom | Zoom-to-Selection——Z 键缩放至选区，Ctrl+Shift+F 恢复完整视图 | ✅ |
| 2026-05-02 | P3 Loop | 循环/AB复读播放——LoopManager + LoopOverlay 半透明高亮，L 键切换，选区或全文件范围内循环，构建通过 | ✅ |
| 2026-05-02 | P3 Crossfade | 选区静音自动交叉淡化——5ms线性淡入淡出边界，消除咔嗒声，Undo兼容 | ✅ |
| 2026-05-02 | P3 Grid | Snap-to-Grid——G 键切换，默认120BPM/4分音符网格，选区吸附到节拍网格，网格线可视化（节拍线7%透明度+小节线15%透明度），与零交叉吸附可协同使用 | ✅ |
| 2026-05-11 | P4-001 | 文件信息显示——TransportBar 显示采样率/位深度/声道数/文件名 | ✅ |
| 2026-05-11 | P4-003 | 频谱视图长文件优化——视口驱动惰性 STFT，移除 10k 帧硬上限，支持任意时长音频 | ✅ |
| 2026-05-14 | 审计修复 | 审计 Open Edison 基础功能，修复选择拖动渲染问题，新增垂直缩放、Save As，修复 Region 删除快捷键 | ✅ |
| 2026-05-14 | P4++ 审计修复 | 选区信息被 TransportBar 遮盖修复，添加 Escape Stop 快捷键，修复频谱图像素间隙 | ✅ |
| 2026-05-14 | P4++ 审计修复(二轮) | TransportBar 选区信息真正显示（之前被按钮遮挡），传递 GridManager 到 TransportBar 显示 BPM，删除 SelectionOverlay 死代码 | ✅ |
| 2026-05-14 | P4+++ 审计修复(三轮) | Undo 历史未清空 Bug（加载新文件后旧操作可破坏新文件）、Ctrl+O 键冲突（Open 与 Fade Out 共用）、功能完整性审计 | ✅ |

## 审计报告 (2026-05-14)

### 发现的问题

| # | 问题 | 严重度 | 状态 |
|---|------|--------|------|
| 1 | **选区拖动时波形高亮和信息文本不更新** — SelectionOverlay::mouseDrag/mouseDown 只对自己调用 repaint()，但选区实际渲染在 WaveformThumbnail 中，信息文本在 MainComponent 中。拖动时视觉无反馈。 | **高** | ✅ 已修复（首轮） |
| 2 | **缺少保存/导出完整音频功能** — 只能通过 Alt+拖拽导出选区 WAV，无法保存编辑后的完整音频文件。 | **高** | ✅ 已修复 — 添加了"Save As..."菜单项和 Ctrl+Shift+S 快捷键（首轮） |
| 3 | **缺少垂直缩放** — WaveformThumbnail 没有垂直缩放控制，无法放大查看波形细节。 | **中** | ✅ 已修复 — 添加了 Ctrl+Shift+鼠标滚轮 垂直缩放（首轮） |
| 4 | **cmdRemoveRegion 没有快捷键** — 这是唯一没有键盘快捷键的已注册命令。 | **低** | ✅ 已修复 — 添加 Shift+Backspace（首轮） |
| 5 | **Region 标签栏在频谱视图中依然可见** — 切换到频谱视图时 RegionOverlay 未隐藏。 | **低** | 不影响功能，区域信息在两种视图中都有用，无需修复 |
| 6 | **选区信息文字被 TransportBar 不透明背景遮盖** — MainComponent::paint() 在底部 36px 绘制选区时间和吸附模式文字，但 TransportBar::paint() 用 fillAll(0xFF222244) 完全覆盖该区域。选区信息对用户永远不可见。 | **中** | ✅ 已修复 — 将选区信息绘制迁移到 TransportBar::paint() 左侧区域，与进度条和信息面板无重叠 |
| 7 | **缺少 Stop 快捷键** — cmdStop 注册在菜单中但没有键盘绑定。Edison 中 Escape 可以停止播放并复位到起点。 | **低** | ✅ 已修复 — 添加 Escape 键映射到 cmdStop |
| 8 | **频谱图像素行间隙** — 当视图高度 > FFT bins (512) 时 (如 800px 窗口)，每 bin 只画单个像素行，出现可视横纹间隙。 | **低** | ✅ 已修复 — 改为按 Y 坐标迭代，寻找最近频率仓，保证每个像素行都有颜色 |
| 9 | **TransportBar 选区信息被按钮覆盖不可见** — drawSelectionInfo 画在 `getLocalBounds().removeFromLeft(136)` 区域，但按钮占用前 124px。文字完全被不透明按钮遮盖。 | **中** | ✅ 已修复 — 改为画在按钮和右侧信息面板之间的中间区域 |
| 10 | **SelectionOverlay::drawSelectionInfo 死代码** — 定义在头文件和实现文件，但从未被调用。TransportBar 有独立的 drawSelectionInfo 负责渲染。 | **低** | ✅ 已修复 — 删除声明和实现 |
| 11 | **TransportBar 缺少 GridManager 引用** — 只能显示 "Grid" 标签，无法显示 BPM/节拍信息。之前通过 SelectionOverlay 的 drawSelectionInfo（死代码）展示了完整信息。 | **低** | ✅ 已修复 — 添加 setGridManager + m_gridManager 成员，drawSelectionInfo 显示 "Grid 120/4" 格式 |

### 修复详情（本轮审计 — 三轮审计新增/修正的问题 #12~#13）

#### 1. Undo 历史未在加载新文件时清空（严重 Bug）
- **文件：** `MainComponent.cpp` `loadAudioFile()`
- **根因：** 加载新文件时，`m_undoManager` 未被清空。如果用户加载文件 B 后按 Ctrl+Z，UndoManager 会尝试用文件 A 的原始样本来修改文件 B 的缓冲区，导致数据损坏。
- **修复：** 在 `loadAudioFile()` 中添加 `m_undoManager.clearUndoHistory()`，确保新文件的撤销栈从干净状态开始。

#### 2. Ctrl+O 键冲突 — Open 与 Fade Out 共用同一快捷键（中优先级）
- **文件：** `MainComponent.cpp` `getCommandInfo()`
- **根因：** `cmdOpen` 和 `cmdFadeOut` 的 `getCommandInfo()` 都调用 `info.addDefaultKeypress('o', ctrlModifier)`。`resetToDefaultMappings()` 将同一个 Ctrl+O 绑定到两个命令上，按 Ctrl+O 时行为不可预测。
- **修复：** 移除 `cmdFadeOut` 的默认快捷键绑定（Fade Out 在菜单中仍可用，仅移除键盘快捷键以避免冲突）。

### 审计发现（无需修复）

| # | 发现 | 说明 |
|---|------|------|
| 1 | 频谱视图无选区叠加层 | `setViewMode(SpectrogramView)` 隐藏 `SelectionOverlay`。Edison 允许在频谱中选区，但 Open Edison 的选区操作设计为波形视图专属——设计意图，非缺陷 |
| 2 | 无时间尺/刻度标尺 | Edison 在波形上方显示秒/节拍刻度。当前 Open Edison 未实现——已计划在 `project_state.md`

#### 1. TransportBar 选区信息被按钮覆盖（中优先级，二轮审计发现）
- **文件：** `TransportBar.cpp`
- **根因：** `TransportBar::paint()` 中 `constexpr int btnWidth = 128` 和 `getLocalBounds().removeFromLeft(btnWidth + 8)` 声称是按钮区域，但 `resized()` 中按钮实际占用 `4 × 30 + 4 gap + 2×2 padding = ~124px`。136px vs 124px 仅有 12px 差异，但文字以 `centredLeft` 对齐，起始于 x=0，完全被 x=4..124 的按钮覆盖。
- **修复：** `drawSelectionInfo` 改为在中间区域渲染：按钮之后（x≥132）、右侧信息面板之前。计算方式：`getLocalBounds().withTrimmedLeft(132).withTrimmedRight(rightPanelWidth + 4)`。

#### 2. 删除 SelectionOverlay::drawSelectionInfo 死代码（低优先级，代码整洁）
- **文件：** `SelectionOverlay.h`, `SelectionOverlay.cpp`
- **根因：** `drawSelectionInfo` 声明在头文件、实现在 cpp，但从未被任何代码调用。TransportBar 有自己独立的 `drawSelectionInfo` 负责渲染选区信息。
- **修复：** 从 SelectionOverlay.h/cpp 中移除该方法的声明和实现。

#### 3. TransportBar 接入 GridManager 显示 BPM 信息（低优先级，信息完整）
- **文件：** `TransportBar.h`, `TransportBar.cpp`, `MainComponent.cpp`
- **根因：** TransportBar 的 `drawSelectionInfo` 只能显示 "Grid" 标签，无法显示 BPM/division（120/4）。而 SelectionOverlay 的（已删除）死代码可以显示完整信息因为它持有 GridManager 引用。
- **修复：** TransportBar 添加 `setGridManager(GridManager*)` 方法和 `m_gridManager` 成员。`drawSelectionInfo` 改为在有 GridManager 引用时显示 "Grid 120/4" 格式。在 `MainComponent` 构造中同步设置 `m_transport->setGridManager(m_gridManager.get())`。

### 旧修复详情（本轮）

#### 1. 选区信息被 TransportBar 遮盖（中优先级）
- **文件：** `MainComponent.cpp`, `TransportBar.h/.cpp`
- **根因：** MainComponent::paint() 在底部 36px 区域绘制选区时间和吸附模式。TransportBar 作为子组件以不透明背景 fillAll 绘制在相同区域，完全覆盖选区信息。
- **修复：** 移除 MainComponent::paint() 中的选区信息绘制。在 TransportBar::paint() 中增加左侧选区信息面板，与按钮区重叠但不冲突。（注意：Grid BPM 信息暂不显示，因为 TransportBar 不持有 GridManager 引用）

#### 2. Stop 快捷键（低优先级，Edison 兼容性）
- **文件：** `MainComponent.cpp`
- **修改：** key mappings 中添加 Escape → cmdStop；getCommandInfo 中添加 Escape 默认键描述

#### 3. 频谱图像素间隙修复（低优先级，视觉质量）
- **文件：** `SpectrogramComponent.cpp`
- **根因：** 原代码按 bin 索引遍历 (0..511) 并映射到 Y 坐标，当 viewHeight > 512 时 bin 间有像素行间隙
- **修复：** 反向迭代：遍历 Y (0..viewHeight-1)，用逆映射找到最近的 bin，确保每行都有颜色

### 已知 Bug

（暂无）

### 功能对比：FL Studio Edison vs Open Edison

| Edison 功能 | Open Edison | 备注 |
|-------------|-------------|------|
| 音频加载/波形显示 | ✅ | WAV/MP3/FLAC |
| 播放/暂停/停止 | ✅ | Space 切换 |
| 选区播放 | ✅ | 播放仅限选区 |
| 选中区域操作（静音/反向/归一化/淡入淡出） | ✅ | 已全部实现 |
| 撤销/重做 | ✅ | Ctrl+Z/Ctrl+Shift+Z |
| 录音 | ✅ | 支持输入设备选择 |
| 频谱分析 | ✅ | 视口驱动 STFT |
| 标记/提示点 | ✅ | M/Shift+M/[ / ] |
| Region/块管理 | ✅ | Ctrl+Shift+R 添加 |
| AB循环复读 | ✅ | L 键切换 |
| 缩放至选区 | ✅ | Z 键，恢复 Ctrl+Shift+F |
| 吸附到网格/零交叉 | ✅ | G/X 键切换 |
| 零交叉吸附+网格可协同 | ✅ | 先网格再零交叉细化 |
| 选区拖拽导出 WAV | ✅ | Alt+选区拖动 |
| 复制/剪切/粘贴 | ✅ | Ctrl+C/X/V |
| **保存/导出编辑后音频** | ✅ | **新增** — Ctrl+Shift+S |
| **垂直缩放** | ✅ | **新增** — Ctrl+Shift+滚轮 |
| **选区拖动视觉反馈** | ✅ | **修复** — 实时更新波形高亮和信息 |
| **Region 删除快捷键** | ✅ | **修复** — 新增 Shift+Backspace |
| 时间尺/刻度标尺 | ❌ | 未实现（计划中） |
| 音量/增益实时控制 | ❌ | 未实现（计划中） |
| 电平表 | ❌ | 未实现（计划中） |
| DC 偏移移除 | ❌ | 未实现 |
| 相位翻转 | ❌ | 未实现 |
| 时间伸缩/变调 | ❌ | 高级功能，暂未实现 |
| 鼠标框选微调（Shift+方向键） | ❌ | 未实现 |

### 修复详情

#### 1. 选区拖动视觉反馈（高优先级 Bug）
- **文件：** `SelectionOverlay.cpp`
- **根因：** mouseDown/mouseDrag 只调用了 `this->repaint()`。但选区的蓝色高亮矩形由 `WaveformThumbnail::paint()` 绘制，信息文本在 `MainComponent::paint()` 中绘制。自 repaint 不会触发它们。
- **修复：** mouseDown/mouseDrag 同时调用 `m_thumbnail.repaint()` 和 `getParentComponent()->repaint()`，确保波形高亮和信息文本实时更新。

#### 2. Save As（新增功能）
- **文件：** `MainComponent.h/cpp`
- **新增命令：** `cmdSaveAs` (Ctrl+Shift+S)
- **逻辑：** 使用文件选择器，以 WAV 格式写入当前缓冲区。默认文件名继承当前文件。更新窗口标题反映已保存的文件名。支持 Undo 兼容（保存整个缓冲区，不影响内存中的可撤销操作）。
- **菜单：** 文件菜单中的 "Save As..."，位于 "Open..." 下方。

#### 3. 垂直缩放（新增功能）
- **文件：** `WaveformThumbnail.h/cpp`
- **新增 API：** `setVerticalZoom(double)`, `getVerticalZoom()`
- **交互：** Ctrl+Shift+鼠标滚轮 — 向上滚放大（1.2x），向下缩小（1/1.2x），范围 0.1x–100x，限制最大幅度不超过声道高度。
- **设计：** 保留 m_zoom（水平缩放）不变，独立控制垂直放大倍数。

#### 4. Region 删除快捷键（新增）
- **文件：** `MainComponent.cpp`
- **新增：** Shift+Backspace 删除当前 Region
- **注意：** 不与 Silence（Delete / Backspace）冲突，因为 Shift 修饰符不同。

## 版本里程碑

### P0 — 可用的基础音频编辑器
- [x] CMake + JUCE 8 构建系统
- [x] 加载 WAV/MP3/FLAC + 波形渲染显示（AudioFileManager + WaveformThumbnail）
- [x] 播放/暂停/停止/选区播放（TransportBar 框架）
- [x] 鼠标框选选区 + 拖动调整边界（SelectionOverlay）
- [x] 选区拖拽导出 WAV（Alt+选区内拖动导出，修复 44100 硬编码 Bug）
- [x] 水平缩放 + 垂直缩放（WaveformThumbnail — 新增垂直缩放）
- [x] 录音（TransportBar 录音回调框架）

### P1 — 接近 Edison 核心体验
- [x] 频谱/声谱图视图（STFT 实时分析，视口驱动惰性渲染）
- [x] 撤销/重做（UndoManager + AudioModifyAction 泛型框架）
- [x] 选区内处理：静音（Silence Selection, Ctrl+D）
- [x] 选区内处理：反向 / 归一化 / 淡入淡出

### P2 — 增强功能（按需）
- [x] 多 Region 管理（Edison 的"块"概念）
- [x] Zoom-to-Selection（Z 键缩放至选区，Ctrl+Shift+F 恢复完整视图）
- [x] 鼠标滚轮精细缩放（Ctrl+滚轮缩放，滚轮水平滚动），Waveform & Spectrogram 双视图
- [x] 标记/提示点（M/Shift+M/[/]快捷键，青色标记旗标，点击跳转）
- [x] 快捷键绑定（空格播放/暂停，Delete/Backspace 静音）

### P3 — 工作流与专业功能
- [x] 循环/AB复读（Loop/Cycle Playback：L 键切换，从选区或全文件建立循环范围）
- [x] 零交叉吸附（Selection Snapping to Zero Crossings）
- [x] 选区操作交叉淡化（Crossfade on Selection Editing）
- [x] 吸附网格（Snap-to-Grid：节拍/拍子对齐）

### P4 — 信息显示与优化 ✅
- [x] 文件信息显示（TransportBar 显示采样率/位深度/声道数/文件名）
- [x] 频谱视图长文件优化（视口驱动惰性 STFT，移除 10k 帧硬上限，支持任意时长音频）

### P4+ — 审计修复 ✅
- [x] 选区拖动视觉反馈实时更新
- [x] 新增垂直缩放（Ctrl+Shift+滚轮）
- [x] 新增 Save As（Ctrl+Shift+S）
- [x] Region 删除快捷键（Shift+Backspace）

## 已知 Bug

（暂无）

## 待处理

1. **P0 DragExport** — 骨架完成，需在 Windows 实际测试拖拽交互（当前 Linux 环境无法验证）。
2. **P4-002: Windows 实机测试** — ⚠️ 需物理 Windows 机器。验证 DragExport、录音、Save As 等功能在 Windows 上的表现。

## P4+ 后续计划

| 功能 | 优先级 | 说明 |
|------|--------|------|
| **时间尺/刻度标尺** | 中 | 在波形上方显示秒/节拍刻度参考线 |
| **音量/增益实时控制** | 中 | TransportBar 添加音量滑块 |
| **电平表** | 低 | 播放时显示实时电平 |
| **DC 偏移移除** | 低 | 常见的音频清理功能 |
| **相位翻转** | 低 | 选区/全文件相位反转 |
| **选区扩展（Shift+方向键）** | 低 | 类似 Edison 逐采样点扩展选区 |
| **F2 重命名 Region** | 低 | 在 Region 标签上双击重命名 |

## 架构设计

```
open-edison/
├── CMakeLists.txt          # 根 CMake（FetchContent JUCE）
├── Source/
│   ├── Main.cpp            # JUCE 入口
│   ├── MainComponent.h/cpp # 主窗口 + 波形显示 + 控制栏 + 命令处理 + Save As
│   ├── AudioFileManager.h/cpp    # 音频文件管理
│   ├── SelectionManager.h        # 选区状态管理
│   ├── SelectionOverlay.h/cpp    # 选区叠加层渲染 + 鼠标交互
│   ├── TransportBar.h/cpp        # 播放控制条
│   ├── WaveformThumbnail.h/cpp   # 波形缩略图渲染（含垂直缩放）
│   ├── DragExport.h/cpp          # 拖拽导出核心
│   ├── SpectrogramComponent.h/cpp # 频谱/声谱图 STFT 渲染（视口驱动惰性渲染）
│   ├── UndoableActions.h/cpp     # Undo/Redo 动作（AudioModifyAction）
│   ├── MarkerManager.h           # 标记点管理（增删改查）
│   ├── MarkerOverlay.h/cpp       # 标记点覆盖层渲染（青色旗标+竖线+标签）
│   ├── LoopManager.h             # 循环播放状态管理（启用/停止/范围）
│   ├── LoopOverlay.h/cpp         # 循环范围覆盖层渲染（半透明高亮+边界线）
│   ├── GridManager.h             # 网格/吸附数据（BPM/拍号）
│   ├── GridOverlay.h/cpp         # 网格覆盖层渲染
│   ├── RegionManager.h/cpp       # 多 Region 管理（Edison 块概念）
│   ├── RegionOverlay.h/cpp       # Region 标签栏渲染 + 点击/右键菜单
├── .gitignore
├── project_state.md
└── README.md
```
