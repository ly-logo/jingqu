import type { CongestionLevel, PoiCategory } from '@/api/types'

export const LEVEL_COLORS: Record<CongestionLevel, string> = {
  EMPTY: '#909399',
  NORMAL: '#67C23A',
  MODERATE: '#E6A23C',
  CROWDED: '#FF7A45',
  OVERLOADED: '#F56C6C',
}

/** 设施点位类别配色（用户端底图 / 列表共用） */
export const POI_CATEGORY_COLORS: Record<PoiCategory, string> = {
  TOILET: '#67C23A',
  PARKING: '#409EFF',
  STATION: '#E6A23C',
  SERVICE: '#909399',
  MEDICAL: '#F56C6C',
  GATE: '#F56C6C',
}

/** 设施点位类别图标 */
export const POI_CATEGORY_ICONS: Record<PoiCategory, string> = {
  TOILET: '🚻',
  PARKING: '🅿',
  STATION: '🚌',
  SERVICE: '🛎',
  MEDICAL: '⛑',
  GATE: '🛂',
}

export const LEVEL_LABELS: Record<CongestionLevel, string> = {
  EMPTY: '空闲',
  NORMAL: '正常',
  MODERATE: '适中',
  CROWDED: '拥挤',
  OVERLOADED: '超载',
}

export function getLevelColor(level: CongestionLevel): string {
  return LEVEL_COLORS[level] || '#909399'
}

export function getLevelLabel(level: CongestionLevel): string {
  return LEVEL_LABELS[level] || level
}

export function ratioToLevel(ratio: number): CongestionLevel {
  if (ratio < 0.3) return 'EMPTY'
  if (ratio < 0.6) return 'NORMAL'
  if (ratio < 0.9) return 'MODERATE'
  if (ratio < 1.0) return 'CROWDED'
  return 'OVERLOADED'
}
