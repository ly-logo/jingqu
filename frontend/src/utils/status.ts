import type {
  EventType,
  EventLevel,
  EventStatus,
  SuggestionType,
  SuggestionStatus,
  RuleMetric,
  SimulationPattern,
} from '@/api/types'

/** 告警等级色板（与设计方案 3.1 一致） */
export const EVENT_LEVEL_COLORS: Record<EventLevel, string> = {
  INFO: '#909399',
  WARNING: '#E6A23C',
  HIGH: '#FF7A45',
  CRITICAL: '#F56C6C',
}

export const EVENT_LEVEL_LABELS: Record<EventLevel, string> = {
  INFO: '提示',
  WARNING: '警告',
  HIGH: '严重',
  CRITICAL: '紧急',
}

/** Element Plus tag type 映射（告警等级） */
export const EVENT_LEVEL_TAG: Record<EventLevel, 'info' | 'warning' | 'danger'> = {
  INFO: 'info',
  WARNING: 'warning',
  HIGH: 'danger',
  CRITICAL: 'danger',
}

export const EVENT_TYPE_LABELS: Record<EventType, string> = {
  CONGESTION: '拥堵',
  OVERLOAD: '超载',
  PARKING_SHORTAGE: '停车不足',
  CAPACITY_SHORTAGE: '运力不足',
  DATA_ANOMALY: '数据异常',
  SYSTEM_ERROR: '系统异常',
}

export const EVENT_STATUS_LABELS: Record<EventStatus, string> = {
  ACTIVE: '待处理',
  ACKNOWLEDGED: '已确认',
  RESOLVED: '已恢复',
  CLOSED: '已关闭',
  IGNORED: '已忽略',
}

export const EVENT_STATUS_TAG: Record<EventStatus, 'info' | 'warning' | 'success' | 'danger'> = {
  ACTIVE: 'danger',
  ACKNOWLEDGED: 'warning',
  RESOLVED: 'success',
  CLOSED: 'info',
  IGNORED: 'info',
}

export const SUGGESTION_TYPE_LABELS: Record<SuggestionType, string> = {
  DIVERSION: '游客分流',
  SHUTTLE_DISPATCH: '摆渡车调配',
  PARKING_GUIDE: '停车引导',
  ENTRANCE_LIMIT: '入口限流',
  RESOURCE_WARNING: '资源预警',
}

export const SUGGESTION_STATUS_LABELS: Record<SuggestionStatus, string> = {
  PENDING: '待执行',
  EXECUTED: '已执行',
  REJECTED: '已驳回',
}

export const SUGGESTION_STATUS_TAG: Record<SuggestionStatus, 'info' | 'warning' | 'success' | 'danger'> = {
  PENDING: 'warning',
  EXECUTED: 'success',
  REJECTED: 'info',
}

export const RULE_METRIC_LABELS: Record<RuleMetric, string> = {
  visitor_ratio: '拥挤度（在园/容量）',
  parking_free: '剩余车位',
  shuttle_load: '摆渡车满载率',
  arrival_rate: '到达速率',
}

export const SIMULATION_PATTERN_LABELS: Record<SimulationPattern, string> = {
  STEADY: '平稳',
  PEAK: '早晚高峰',
  RANDOM: '随机脉冲',
}

export const ROLE_LABELS: Record<string, string> = {
  admin: '管理员',
  operator: '运营',
  viewer: '游客',
}

export function eventLevelColor(level: EventLevel): string {
  return EVENT_LEVEL_COLORS[level] || '#909399'
}
