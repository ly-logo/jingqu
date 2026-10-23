import type {
  ApiResult,
  EventItem,
  EventType,
  EventLevel,
  EventStatus,
  Suggestion,
  SuggestionType,
  SuggestionStatus,
  Rule,
  User,
  UserCreateRequest,
  StatisticsData,
  SimulationConfig,
  SimulationState,
  SystemMonitor,
  SystemSettings,
  VisitorEntryRequest,
  ParkingEntryRequest,
} from './types'

function delay<T>(data: ApiResult<T>): Promise<ApiResult<T>> {
  return new Promise((resolve) => {
    setTimeout(() => resolve(data), 100 + Math.random() * 200)
  })
}

function ok<T>(data: T): Promise<ApiResult<T>> {
  return delay({ code: 200, message: 'success', data })
}

function fail<T>(code: number, message: string): Promise<ApiResult<T>> {
  return delay({ code, message, data: null as any })
}

function pad(n: number): string {
  return String(n).padStart(2, '0')
}

function fmt(d: Date): string {
  return `${d.getFullYear()}-${pad(d.getMonth() + 1)}-${pad(d.getDate())} ${pad(d.getHours())}:${pad(
    d.getMinutes(),
  )}:${pad(d.getSeconds())}`
}

function minutesAgo(min: number): string {
  return fmt(new Date(Date.now() - min * 60 * 1000))
}

const spotNames = [
  '观瀑台',
  '云海栈道',
  '青石古镇',
  '望湖亭',
  '百花谷',
  '索道站',
  '竹林径',
  '山顶广场',
]

// ==================== 事件 ====================

const eventSeeds: Array<[EventType, EventLevel, number, string]> = [
  ['OVERLOAD', 'CRITICAL', 3, '当前在园 820 人，超过瞬时容量 800 人，已触发超载告警'],
  ['CONGESTION', 'HIGH', 8, '当前在园 595 人，拥挤度 70%，建议启动分流'],
  ['PARKING_SHORTAGE', 'WARNING', 15, '东门停车场剩余车位 12 个，低于阈值 30'],
  ['CONGESTION', 'HIGH', 25, '当前在园 610 人，拥挤度 72%，持续 12 分钟'],
  ['CAPACITY_SHORTAGE', 'WARNING', 40, '索道站排队人数 180，摆渡车运力不足'],
  ['CONGESTION', 'WARNING', 62, '竹林径拥挤度 58%，接近阈值'],
  ['OVERLOAD', 'CRITICAL', 95, '山顶广场在园 1080 人，超过瞬时容量 1000 人'],
  ['PARKING_SHORTAGE', 'WARNING', 130, '西门停车场剩余车位 22 个，低于阈值 30'],
  ['DATA_ANOMALY', 'INFO', 180, '观瀑台流量数据 3 分钟未更新，已自动补位'],
  ['CONGESTION', 'HIGH', 240, '青石古镇拥挤度 74%，持续 15 分钟'],
  ['CAPACITY_SHORTAGE', 'WARNING', 300, '百花谷站点摆渡车满载率 100%，等待时间上升'],
  ['SYSTEM_ERROR', 'CRITICAL', 360, '数据写库线程异常重启，已自动恢复'],
  ['CONGESTION', 'WARNING', 420, '望湖亭拥挤度 61%'],
  ['PARKING_SHORTAGE', 'INFO', 480, '南门停车场剩余车位 120 个，恢复正常'],
  ['OVERLOAD', 'HIGH', 540, '观瀑台在园 840 人，持续 8 分钟后回落'],
  ['CONGESTION', 'WARNING', 640, '云海栈道拥挤度 55%，趋势上升'],
  ['DATA_ANOMALY', 'INFO', 720, '检测到停车场上报数据跳变，已记录'],
  ['CONGESTION', 'HIGH', 800, '山顶广场拥挤度 78%，建议分流至竹林径'],
  ['CAPACITY_SHORTAGE', 'WARNING', 900, '摆渡车 07 号故障，在线运力下降'],
  ['CONGESTION', 'INFO', 1000, '东门入口到达速率 45 人/分钟，高于日常均值'],
]

const eventStatusPool: EventStatus[] = [
  'ACTIVE',
  'ACKNOWLEDGED',
  'RESOLVED',
  'CLOSED',
  'IGNORED',
]

let events: EventItem[] = eventSeeds.map((seed, i) => {
  const [type, level, mins, description] = seed
  const status: EventStatus = i === 0 ? 'ACTIVE' : eventStatusPool[i % eventStatusPool.length]
  const spotIdx = (i * 3) % spotNames.length
  return {
    eventId: 1000 + i,
    type,
    level,
    spotId: `spot_${pad(spotIdx + 1)}`,
    spotName: spotNames[spotIdx],
    description,
    status,
    suggestion:
      type === 'OVERLOAD' || type === 'CONGESTION'
        ? `建议将部分游客分流至${spotNames[(spotIdx + 2) % spotNames.length]}`
        : type === 'PARKING_SHORTAGE'
          ? '建议引导车辆前往南门停车场'
          : type === 'CAPACITY_SHORTAGE'
            ? '建议调派最近空闲摆渡车前往支援'
            : undefined,
    createdAt: minutesAgo(mins),
    updatedAt: status === 'ACTIVE' ? undefined : minutesAgo(Math.max(1, mins - 5)),
  }
})

export const mockEvent = {
  list(params?: { type?: string; level?: string; status?: string }): Promise<ApiResult<EventItem[]>> {
    let list = events.map((e) => ({ ...e }))
    if (params?.type) list = list.filter((e) => e.type === params.type)
    if (params?.level) list = list.filter((e) => e.level === params.level)
    if (params?.status) list = list.filter((e) => e.status === params.status)
    return ok(list)
  },
  updateStatus(eventId: number, status: EventStatus): Promise<ApiResult<null>> {
    const target = events.find((e) => e.eventId === eventId)
    if (!target) return fail(404, '事件不存在')
    target.status = status
    target.updatedAt = fmt(new Date())
    return ok(null)
  },
}

// ==================== 调度建议 ====================

const suggestionSeeds: Array<[SuggestionType, string, string, number]> = [
  ['DIVERSION', '观瀑台 → 竹林径：分流 120 人', '目标景点剩余容量 180，距离 320 米，满足 90% 安全余量约束', 3],
  ['SHUTTLE_DISPATCH', '调派摆渡车 03 号前往索道站', '索道站排队 180 人，03 号当前空闲且距索道站最近（1.2 公里）', 12],
  ['PARKING_GUIDE', '引导车辆前往南门停车场', '东门剩余 12 位、西门 22 位，南门剩余 120 位', 18],
  ['ENTRANCE_LIMIT', '东门入口启动限流', '全景区在园人数达总容量 92%，建议入口放行速率降至 30 人/分钟', 30],
  ['RESOURCE_WARNING', '摆渡车运力预警', '在线 12/15，满载率超过 85% 的车辆有 4 辆', 45],
  ['DIVERSION', '山顶广场 → 云海栈道：分流 200 人', '山顶广场拥挤度 108%，目标景点剩余容量 380', 60],
  ['SHUTTLE_DISPATCH', '调派摆渡车 09 号前往百花谷', '百花谷站点满载率 100%，09 号距站点 0.8 公里', 95],
  ['PARKING_GUIDE', '西门停车场入口引导', '西门剩余车位 22 个，建议引导至东门', 130],
  ['DIVERSION', '青石古镇 → 望湖亭：分流 80 人', '青石古镇拥挤度 74%，望湖亭剩余容量 150', 180],
  ['RESOURCE_WARNING', '停车场整体饱和预警', '全景区剩余车位 214，低于总车位 25%', 240],
  ['ENTRANCE_LIMIT', '南门入口恢复正常放行', '全景区在园人数回落至总容量 78%', 300],
  ['SHUTTLE_DISPATCH', '增开摆渡车加班车 2 趟', '早高峰预测到达率上升，建议提前增开', 360],
]

const suggestionStatusPool = ['PENDING', 'EXECUTED', 'REJECTED'] as const

let suggestions: Suggestion[] = suggestionSeeds.map((seed, i) => {
  const [type, content, detail, mins] = seed
  return {
    suggestionId: 2000 + i,
    type,
    content,
    detail,
    spotId: `spot_${pad(((i * 2) % spotNames.length) + 1)}`,
    status: i === 0 ? 'PENDING' : suggestionStatusPool[i % 3],
    createdAt: minutesAgo(mins),
  }
})

export const mockScheduling = {
  list(): Promise<ApiResult<Suggestion[]>> {
    return ok(suggestions.map((s) => ({ ...s })))
  },
  detail(suggestionId: number): Promise<ApiResult<Suggestion>> {
    const t = suggestions.find((s) => s.suggestionId === suggestionId)
    return t ? ok({ ...t }) : fail(404, '调度建议不存在')
  },
  updateStatus(suggestionId: number, status: SuggestionStatus): Promise<ApiResult<null>> {
    const t = suggestions.find((s) => s.suggestionId === suggestionId)
    if (!t) return fail(404, '调度建议不存在')
    t.status = status
    return ok(null)
  },
}

// ==================== 规则 ====================

let rules: Rule[] = [
  { ruleId: 1, spotId: '*', metric: 'visitor_ratio', operator: '>=', threshold: 0.9, action: '触发拥挤告警', level: 'HIGH', priority: 10, enabled: true },
  { ruleId: 2, spotId: '*', metric: 'visitor_ratio', operator: '>=', threshold: 1.0, action: '触发超载告警并生成分流建议', level: 'CRITICAL', priority: 1, enabled: true },
  { ruleId: 3, spotId: '*', metric: 'arrival_rate', operator: '>', threshold: 40, action: '关注到达速率异常', level: 'INFO', priority: 50, enabled: true },
  { ruleId: 4, spotId: 'spot_01', metric: 'visitor_ratio', operator: '>=', threshold: 0.85, action: '观瀑台提前预警', level: 'WARNING', priority: 20, enabled: true },
  { ruleId: 5, spotId: '*', metric: 'parking_free', operator: '<', threshold: 30, action: '触发停车引导建议', level: 'WARNING', priority: 30, enabled: true },
  { ruleId: 6, spotId: '*', metric: 'shuttle_load', operator: '>=', threshold: 0.95, action: '触发运力不足告警', level: 'WARNING', priority: 35, enabled: true },
  { ruleId: 7, spotId: 'spot_08', metric: 'visitor_ratio', operator: '>=', threshold: 0.95, action: '山顶广场限制进入', level: 'CRITICAL', priority: 5, enabled: true },
  { ruleId: 8, spotId: '*', metric: 'arrival_rate', operator: '<', threshold: 5, action: '数据异常排查', level: 'INFO', priority: 60, enabled: false },
]

let ruleSeq = 100

export const mockRule = {
  list(): Promise<ApiResult<Rule[]>> {
    return ok(rules.map((r) => ({ ...r })))
  },
  create(data: Omit<Rule, 'ruleId'>): Promise<ApiResult<Rule>> {
    const rule: Rule = { ...data, ruleId: ++ruleSeq }
    rules.push(rule)
    return ok(rule)
  },
  update(ruleId: number, data: Partial<Rule>): Promise<ApiResult<Rule>> {
    const t = rules.find((r) => r.ruleId === ruleId)
    if (!t) return fail(404, '规则不存在')
    Object.assign(t, data)
    return ok({ ...t })
  },
  remove(ruleId: number): Promise<ApiResult<null>> {
    const idx = rules.findIndex((r) => r.ruleId === ruleId)
    if (idx < 0) return fail(404, '规则不存在')
    rules.splice(idx, 1)
    return ok(null)
  },
}

// ==================== 用户 ====================

let users: User[] = [
  { userId: 1, username: 'admin', role: 'admin', createdAt: minutesAgo(60 * 24 * 30) },
  { userId: 2, username: 'operator', role: 'operator', createdAt: minutesAgo(60 * 24 * 20) },
  { userId: 3, username: 'operator02', role: 'operator', createdAt: minutesAgo(60 * 24 * 12) },
  { userId: 4, username: 'viewer', role: 'viewer', createdAt: minutesAgo(60 * 24 * 5) },
  { userId: 5, username: 'guest01', role: 'viewer', createdAt: minutesAgo(60 * 24 * 2) },
]

let userSeq = 100

export const mockUser = {
  list(): Promise<ApiResult<User[]>> {
    return ok(users.map((u) => ({ ...u })))
  },
  create(data: UserCreateRequest): Promise<ApiResult<User>> {
    if (users.some((u) => u.username === data.username)) {
      return fail(409, '用户名已存在')
    }
    const user: User = {
      userId: ++userSeq,
      username: data.username,
      role: data.role,
      createdAt: fmt(new Date()),
    }
    users.push(user)
    return ok(user)
  },
  remove(userId: number): Promise<ApiResult<null>> {
    const idx = users.findIndex((u) => u.userId === userId)
    if (idx < 0) return fail(404, '用户不存在')
    if (users[idx].username === 'admin') return fail(403, '内置管理员不可删除')
    users.splice(idx, 1)
    return ok(null)
  },
}

// ==================== 统计 ====================

const flowStats = Array.from({ length: 7 }, (_, i) => {
  const d = new Date(Date.now() - (6 - i) * 24 * 60 * 60 * 1000)
  return {
    date: `${pad(d.getMonth() + 1)}-${pad(d.getDate())}`,
    visitors: Math.round(9000 + Math.random() * 6000),
  }
})

const congestionStats = spotNames.map((name, i) => ({
  spotName: name,
  crowdedMinutes: Math.round((8 - i) * 35 + Math.random() * 60),
}))

const resourceStats = Array.from({ length: 12 }, (_, i) => ({
  time: `${pad(i * 2)}:00`,
  utilization: Math.round(40 + 40 * Math.sin((i / 12) * Math.PI) + Math.random() * 10),
}))

const suggestionTypeCount = (
  ['DIVERSION', 'SHUTTLE_DISPATCH', 'PARKING_GUIDE', 'ENTRANCE_LIMIT', 'RESOURCE_WARNING'] as SuggestionType[]
).map((type, i) => ({ type, count: Math.round(20 - i * 3 + Math.random() * 5) }))

export const mockStatistics = {
  overview(): Promise<ApiResult<StatisticsData>> {
    return ok({
      flow: flowStats.map((f) => ({ ...f })),
      congestion: congestionStats.map((c) => ({ ...c })),
      resource: resourceStats.map((r) => ({ ...r })),
      suggestionTypeCount: suggestionTypeCount.map((s) => ({ ...s })),
    })
  },
}

// ==================== 模拟 ====================

let simConfig: SimulationConfig = { arrivalRate: 45, pattern: 'PEAK', durationMinutes: 120 }
let simRunning = false

export const mockSimulation = {
  state(): Promise<ApiResult<SimulationState>> {
    return ok({ running: simRunning, config: { ...simConfig } })
  },
  start(): Promise<ApiResult<SimulationState>> {
    simRunning = true
    return ok({ running: true, config: { ...simConfig } })
  },
  stop(): Promise<ApiResult<SimulationState>> {
    simRunning = false
    return ok({ running: false, config: { ...simConfig } })
  },
  updateConfig(config: Partial<SimulationConfig>): Promise<ApiResult<SimulationConfig>> {
    simConfig = { ...simConfig, ...config }
    return ok({ ...simConfig })
  },
}

// ==================== 系统监控 ====================

export const mockMonitor = {
  status(): Promise<ApiResult<SystemMonitor>> {
    return ok({
      pipelines: [
        { name: '数据接入线程', running: true, queueDepth: Math.round(Math.random() * 20) },
        { name: '预处理线程', running: true, queueDepth: Math.round(Math.random() * 15) },
        { name: '拥挤计算线程', running: true, queueDepth: Math.round(Math.random() * 10) },
        { name: '规则引擎线程', running: true, queueDepth: Math.round(Math.random() * 8) },
        { name: '数据写库线程', running: true, queueDepth: Math.round(Math.random() * 30) },
      ],
      sources: [
        { name: '手动录入', status: simRunning ? '已暂停（模拟中）' : '正常' },
        { name: '模拟生成器', status: simRunning ? '运行中' : '已停止' },
        { name: '文件导入', status: '就绪' },
        { name: '闸机接口', status: '未接入（预留）' },
        { name: '停车场接口', status: '未接入（预留）' },
      ],
      logs: [
        { time: minutesAgo(1), level: 'INFO', message: '模拟数据生成器已启动，到达率 45 人/分钟' },
        { time: minutesAgo(3), level: 'WARN', message: '观瀑台拥挤度达到 90%，触发 HIGH 告警' },
        { time: minutesAgo(8), level: 'ERROR', message: '写库批量任务超时，已自动重试成功' },
        { time: minutesAgo(15), level: 'INFO', message: '规则引擎加载 8 条规则' },
        { time: minutesAgo(30), level: 'INFO', message: '数据库连接池初始化完成（连接数 4）' },
      ],
    })
  },
}

// ==================== 系统设置 ====================

let settings: SystemSettings = {
  totalCapacity: 20000,
  debounceSeconds: 30,
  retentionDays: 7,
  batchIntervalMs: 1000,
}

export const mockSettings = {
  get(): Promise<ApiResult<SystemSettings>> {
    return ok({ ...settings })
  },
  update(data: Partial<SystemSettings>): Promise<ApiResult<SystemSettings>> {
    settings = { ...settings, ...data }
    return ok({ ...settings })
  },
}

// ==================== 数据录入 ====================

export const mockEntry = {
  submitVisitor(req: VisitorEntryRequest): Promise<ApiResult<null>> {
    if (simRunning) return fail(409, '模拟运行中，禁止手动录入游客数')
    if (req.currentVisitors < 0) return fail(400, '游客数不能为负')
    return ok(null)
  },
  submitParking(req: ParkingEntryRequest): Promise<ApiResult<null>> {
    if (simRunning) return fail(409, '模拟运行中，禁止手动录入停车数据')
    if (req.occupiedSpaces < 0) return fail(400, '占用车位数不能为负')
    return ok(null)
  },
}
