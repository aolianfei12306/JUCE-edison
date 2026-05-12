# Open Edison 测试文档

> 版本：0.1.0 | 更新日期：2026-05-12

---

## 1. 测试框架

使用 **JUCE 内置的 UnitTest 系统**（`juce::UnitTest`），无任何外部测试依赖。

### 运行方式

```bash
# Linux
cd ~/Projects/open-edison
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target OpenEdison_Tests -j$(nproc)
./build/OpenEdison_Tests
```

### 预期输出

所有测试通过时：
```
JUCE v8.0.5
Completed tests in AudioFileManager / ...
Completed tests in GridManager / ...
Completed tests in LoopManager / ...
Completed tests in MarkerManager / ...
Completed tests in RegionManager / ...
Completed tests in SelectionManager / ...
Completed tests in SnapToZero / ...
========================================
  Open Edison Test Summary
========================================
  Passed: 1
  Failed: 0
========================================
```

> **注**：`Passed: 1` 只统计 "EdisonAudio" 类别内的单个亚像素对齐测试。其余 "EdisonCore" 类别测试的通过状态由无 `FAILED!!` 输出来保证。

如遇测试失败：
```
!!! Test N failed
FAILED!!  X tests failed, out of a total of Y
```

---

## 2. 测试模块详解

### 2.1 AudioFileManagerTest

**测试类别**：`EdisonCore` | **文件**：`Source/tests/TestAudioFileManager.cpp`

| 测试组 | 验证内容 |
|--------|---------|
| `default state` | 无文件加载时：hasAudio=false, numChannels=0, duration=0, buffer=nullptr, thumbnail 存在, fileName 为空, file 不存在 |
| `unload when empty` | 对空管理器执行 unload 不崩溃、不改变状态 |

**测试重点**：
- 验证构造后的默认状态一致
- 确保空状态下的 unload 操作安全（无崩溃）

---

### 2.2 GridManagerTest

**测试类别**：`EdisonCore` | **文件**：`Source/tests/TestGridManager.cpp`

| 测试组 | 验证内容 |
|--------|---------|
| `default state` | enabled=false, BPM=120, division=4 |
| `toggle enabled` | 开关功能正常 |
| `BPM clamping` | BPM 被限制在 [20, 500] |
| `division clamping` | division 被限制在 [1, 32] |
| `beat duration` | 120BPM=0.5s, 60BPM=1.0s, 240BPM=0.25s |
| `grid interval` | division=4 → 0.5s, 8→0.25s, 16→0.125s, 1→2.0s |
| `snap to grid` | 吸附精度、边界限制（0~duration）、开关控制 |
| `get grid lines` | 线生成正确，起始位置、步长正确 |
| `get bar lines` | 小节线生成正确，4拍/小节 |
| `edge cases` | 负值预期到 0、超限预期到 duration、BPM下限 |

**测试重点**：
- 数值范围内的边界条件
- 吸附算法的正确性（到哪个网格点）
- 网格线和节拍线的生成算法

---

### 2.3 LoopManagerTest

**测试类别**：`EdisonCore` | **文件**：`Source/tests/TestLoopManager.cpp`

| 测试组 | 验证内容 |
|--------|---------|
| `default state` | enabled=false, active=false, hasValidLoop=false |
| `enable/disable loop` | 开关不影响 hasValidLoop |
| `set loop range` | 正确设置起止时间 |
| `reversed range` | 正确交换 start/end |
| `active state` | active 状态切换 |
| `disable clears active` | 关闭循环播放时自动清除 active |
| `clear loop` | 清除后全部重置 |
| `edge cases` | 零长度、极小长度循环被拒绝（<0.001s） |

**测试重点**：
- 状态转换的边界条件
- 无效循环范围的拒绝（长度过小）
- disable/clear 对 active 状态的影响

---

### 2.4 MarkerManagerTest

**测试类别**：`EdisonCore` | **文件**：`Source/tests/TestMarkerManager.cpp`

| 测试组 | 验证内容 |
|--------|---------|
| `default state` | numMarkers=0, getMarkers() 为空 |
| `add markers` | 正确添加、自动按时间排序、标签递增（M1/M2/M3） |
| `get marker at position` | 精确匹配、容差范围内的匹配、超容差返回 -1 |
| `navigation` | getNextMarker/getPrevMarker 按时间跳转 |
| `remove marker by position` | 按时间移除、容差控制 |
| `remove marker by ID` | 按 ID 移除 |
| `clear` | 清除后 numMarkers=0，ID 重置为 1 |
| `marker labels` | 标签格式正确（"M2 @ 1.00s"） |

**测试重点**：
- 按时间自动排序
- 时间到 ID 的双向映射
- 导航（上/下一个标记）
- 清除后 ID 重置

---

### 2.5 RegionManagerTest

**测试类别**：`EdisonCore` | **文件**：`Source/tests/TestRegionManager.cpp`

| 测试组 | 验证内容 |
|--------|---------|
| `default state` | numRegions=0 |
| `add regions` | 添加 3 个 region、ID 递增、按 startTime 排序 |
| `get region at time` | 时间点命中区域、不命中区域返回 -1、负时间 |
| `get region by ID` | 按 ID 查找、不存在的 ID 返回 nullptr |
| `rename region` | 重命名功能 |
| `navigation` | 上/下一个 region（自动环绕：最后一个回到第一个） |
| `remove region` | 按 ID 移除 |
| `remove region at time` | 按时间点移除 |
| `current region ID` | 当前选择区域设置与读取 |
| `clear` | 清除后全部重置 |

**测试重点**：
- 添加后自动排序
- 时间命中检测
- 导航环绕行为（到达末尾后回到开头）
- 按时间和按 ID 移除

---

### 2.6 SelectionManagerTest

**测试类别**：`EdisonCore` | **文件**：`Source/tests/TestSelectionManager.cpp`

| 测试组 | 验证内容 |
|--------|---------|
| `default state` | hasSelection=false, duration=0, position=0, snap 开关关闭 |
| `set and clear selection` | 设置后 hasSelection、起止时间、时长正确 |
| `set selection reversed order` | 自动交换 start/end |
| `minimum selection threshold` | <0.001s 的选择被视为无效 |
| `selection duration` | 10s 的选择 duration=10s，clear 后=0 |
| `playback position` | 位置读写 |
| `total duration` | 总时长读写 |
| `snap toggling` | snap-to-zero 和 snap-to-grid 开关状态 |
| `snap time with grid` | 网格吸附计算（依赖 AudioFileManager 和 GridManager） |

**测试重点**：
- 选择区域的时间准确性和容差
- 反转起止时间的自动修正
- 极短选择区间的拒绝
- 网格吸附的集成行为

---

### 2.7 SnapToZeroTest

**测试类别**：`EdisonAudio` | **文件**：`Source/tests/TestAudioFileManager.cpp`

| 测试组 | 验证内容 |
|--------|---------|
| `snap with no audio loaded returns identity` | 无音频时返回原始时间 |

**测试重点**：
- 空缓冲区的安全调用（不崩溃）

---

## 3. 测试架构说明

### 测试隔离

测试目标 `OpenEdison_Tests` 与主程序 `OpenEdison` 完全独立构建：

```
OpenEdison_Tests（独立可执行文件）
    ├── Source/tests/*.cpp       # 测试用例
    ├── Source/AudioFileManager.cpp   # 被测试的源文件（非 GUI）
    └── Source/RegionManager.cpp      # 被测试的源文件（非 GUI）
```

不链接任何 GUI 组件（MainComponent、WaveformThumbnail 等），因此测试可在无显示器的环境下运行。

### 测试类别

| 类别 | 说明 | 测试数 |
|------|------|--------|
| `EdisonCore` | 核心业务逻辑（7 个测试模块，共 ~50 组） | 运行全部 |
| `EdisonAudio` | 音频相关（1 个测试模块） | 运行全部 |

### 添加新测试

1. 在 `Source/tests/` 下创建 `Test*.cpp`
2. 派生 `juce::UnitTest`，第二个参数指定测试类别（`"EdisonCore"` 或 `"EdisonAudio"`）
3. 在 `runTest()` 中用 `beginTest("name")` 分组，用 `expect()` / `expectEquals()` / `expectWithinAbsoluteError()` 断言
4. 文件底部加静态全局实例
5. 如需链接额外源文件，在 `CMakeLists.txt` 的 `TEST_DEPS_SOURCE` 中添加
