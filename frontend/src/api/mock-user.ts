import type {
  ApiResult,
  OverviewData,
  RankingsData,
  ForecastData,
  UserParking,
  ShuttleRouteGroup,
  Poi,
  PoiCategory,
  RecommendData,
  SpotDetailData,
  UserSpotBrief,
  CongestionLevel,
} from './types'

/** 用户端接口的本地 Mock（VITE_USE_MOCK=true 时启用），字段与真实接口完全一致。 */

function delay<T>(data: ApiResult<T>): Promise<ApiResult<T>> {
  return new Promise((resolve) => {
    setTimeout(() => resolve(data), 100 + Math.random() * 200)
  })
}

function levelOf(ratio: number): CongestionLevel {
  if (ratio < 0.3) return 'EMPTY'
  if (ratio < 0.6) return 'NORMAL'
  if (ratio < 0.9) return 'MODERATE'
  if (ratio < 1.0) return 'CROWDED'
  return 'OVERLOADED'
}

function textOf(level: CongestionLevel): string {
  if (level === 'OVERLOADED' || level === 'CROWDED') return '拥挤'
  if (level === 'MODERATE') return '适中'
  return '空闲'
}

function queueMinutesOf(ratio: number): number {
  if (ratio <= 0) return 0
  return Math.min(60, Math.round(ratio * 60))
}

// 与后端 seed 相同的 8 个景点基线
const spots = [
  { spotId: 'spot_01', name: '观瀑台', x: 22, y: 68, cap: 800, base: 0.95, open: '08:00', close: '18:00' },
  { spotId: 'spot_02', name: '云海栈道', x: 48, y: 82, cap: 900, base: 0.45, open: '08:30', close: '17:30' },
  { spotId: 'spot_03', name: '青石古镇', x: 75, y: 55, cap: 850, base: 0.62, open: '08:00', close: '21:00' },
  { spotId: 'spot_04', name: '望湖亭', x: 60, y: 30, cap: 400, base: 0.35, open: '08:00', close: '18:00' },
  { spotId: 'spot_05', name: '百花谷', x: 35, y: 40, cap: 600, base: 0.5, open: '07:30', close: '18:30' },
  { spotId: 'spot_06', name: '索道站', x: 15, y: 25, cap: 300, base: 0.75, open: '07:30', close: '17:00' },
  { spotId: 'spot_07', name: '竹林径', x: 82, y: 78, cap: 500, base: 0.28, open: '08:00', close: '18:00' },
  { spotId: 'spot_08', name: '山顶广场', x: 50, y: 90, cap: 1000, base: 0.4, open: '08:00', close: '19:00' },
]

function ratioOf(s: (typeof spots)[number]): number {
  return Math.min(1.1, s.base * (0.9 + Math.random() * 0.2))
}

function toBrief(s: (typeof spots)[number], withLocation = false): UserSpotBrief {
  const ratio = ratioOf(s)
  const level = levelOf(ratio)
  const brief: UserSpotBrief = {
    spotId: s.spotId,
    name: s.name,
    level,
    levelText: textOf(level),
    queueMinutes: queueMinutesOf(ratio),
    openTime: s.open,
    closeTime: s.close,
  }
  if (withLocation) {
    brief.locationX = s.x
    brief.locationY = s.y
  }
  return brief
}

const parkings: UserParking[] = [
  { parkingId: 'park_01', name: '东门停车场', freeSpaces: 180, distanceM: 80, latitude: 30.115, longitude: 120.108, statusText: '余位充足' },
  { parkingId: 'park_02', name: '西门停车场', freeSpaces: 90, distanceM: 420, latitude: 30.14, longitude: 120.105, statusText: '余位充足' },
  { parkingId: 'park_03', name: '南门停车场', freeSpaces: 12, distanceM: 650, latitude: 30.108, longitude: 120.145, statusText: '余位紧张' },
]

const routes: ShuttleRouteGroup[] = [
  { routeName: '东门环线', stations: ['东门', '索道站', '观瀑台', '山顶广场'], waitingMinutes: 10, totalDistanceM: 2080 },
  { routeName: '西门环线', stations: ['西门', '百花谷', '望湖亭', '青石古镇'], waitingMinutes: 18, totalDistanceM: 1720 },
]

const pois: Poi[] = [
  { poiId: 'toilet_01', name: '卫生间 · 东门', category: 'TOILET', categoryText: '卫生间', locationX: 12, locationY: 18, latitude: 30.118, longitude: 120.112 },
  { poiId: 'toilet_02', name: '卫生间 · 中心广场', category: 'TOILET', categoryText: '卫生间', locationX: 50, locationY: 50, latitude: 30.15, longitude: 120.15 },
  { poiId: 'toilet_03', name: '卫生间 · 山顶', category: 'TOILET', categoryText: '卫生间', locationX: 52, locationY: 86, latitude: 30.186, longitude: 120.152 },
  { poiId: 'station_01', name: '景区车站 · 东门', category: 'STATION', categoryText: '车站', locationX: 10, locationY: 12, latitude: 30.112, longitude: 120.11 },
  { poiId: 'station_02', name: '景区车站 · 西门', category: 'STATION', categoryText: '车站', locationX: 6, locationY: 42, latitude: 30.142, longitude: 120.106 },
  { poiId: 'park_p01', name: '东门停车场', category: 'PARKING', categoryText: '停车场', locationX: 8, locationY: 15, latitude: 30.115, longitude: 120.108 },
  { poiId: 'park_p02', name: '西门停车场', category: 'PARKING', categoryText: '停车场', locationX: 5, locationY: 40, latitude: 30.14, longitude: 120.105 },
  { poiId: 'park_p03', name: '南门停车场', category: 'PARKING', categoryText: '停车场', locationX: 45, locationY: 8, latitude: 30.108, longitude: 120.145 },
  { poiId: 'service_01', name: '游客服务中心', category: 'SERVICE', categoryText: '服务点', locationX: 48, locationY: 46, latitude: 30.146, longitude: 120.148 },
  { poiId: 'medical_01', name: '医疗救助点', category: 'MEDICAL', categoryText: '医疗点', locationX: 52, locationY: 52, latitude: 30.152, longitude: 120.148 },
]

function forecastPoints(): ForecastData {
  const points = Array.from({ length: 24 }, (_, h) => {
    const predicted = Math.round(8400 * (0.55 + 0.45 * Math.sin(((h - 6) / 24) * 2 * Math.PI)))
    const level = levelOf(predicted / 20000)
    return { hour: h, predictedVisitors: predicted, level, levelText: textOf(level) }
  })
  return { date: new Date().toISOString().slice(0, 10), points, peakStart: 10, peakEnd: 15 }
}

function recommend(type: 'avoid' | 'classic'): RecommendData {
  const pool = spots
    .filter((s) => type === 'classic' || s.base < 0.9)
    .map((s) => ({ s, brief: toBrief(s, true) }))
  // 最近邻串联
  const used = new Set<number>()
  let cur = 0
  let best = Infinity
  pool.forEach((p, i) => {
    if (p.s.x + p.s.y < best) {
      best = p.s.x + p.s.y
      cur = i
    }
  })
  const steps = []
  let totalUnits = 0
  while (used.size < pool.length) {
    used.add(cur)
    const node = pool[cur]
    steps.push({ ...node.brief, order: steps.length + 1 })
    let next = -1
    let nextDist = Infinity
    pool.forEach((p, i) => {
      if (used.has(i)) return
      const d = Math.hypot(p.s.x - node.s.x, p.s.y - node.s.y)
      if (d < nextDist) {
        nextDist = d
        next = i
      }
    })
    if (next < 0) break
    totalUnits += nextDist
    cur = next
  }
  const totalDistanceM = Math.round(totalUnits * 20)
  return {
    type,
    name: type === 'avoid' ? '避峰路线' : '经典路线',
    steps,
    totalDistanceM,
    estimatedMinutes: Math.round(totalDistanceM / 80) + steps.length * 30,
  }
}

export const mockUserSide = {
  overview(): Promise<ApiResult<OverviewData>> {
    const briefs = spots.map((s) => toBrief(s, true))
    const totalCurrent = Math.round(20000 * 0.42)
    const crowded = briefs.filter((b) => b.level === 'CROWDED' || b.level === 'OVERLOADED').length
    const overall = levelOf(totalCurrent / 20000)
    return delay({
      code: 200,
      message: 'success',
      data: {
        conclusion: {
          level: overall,
          text: textOf(overall),
          currentVisitors: totalCurrent,
          totalCapacity: 20000,
          ratio: totalCurrent / 20000,
          crowdedSpotCount: crowded,
        },
        peakAdvice: {
          isPeak: false,
          peakStart: 10,
          peakEnd: 15,
          message: '当前非高峰时段（高峰 10:00~15:00），可放心游览热门景点',
        },
        spots: briefs,
        updatedAt: new Date().toLocaleTimeString('zh-CN', { hour12: false }),
      },
    })
  },
  rankings(type: 'avoid' | 'crowded'): Promise<ApiResult<RankingsData>> {
    const sorted = [...spots]
      .map((s) => ({ s, ratio: ratioOf(s) }))
      .sort((a, b) => (type === 'avoid' ? a.ratio - b.ratio : b.ratio - a.ratio))
      .filter((x) => type === 'avoid' || x.ratio >= 0.6)
      .slice(0, 5)
    return delay({
      code: 200,
      message: 'success',
      data: { type, list: sorted.map((x) => toBrief(x.s)) },
    })
  },
  forecast(): Promise<ApiResult<ForecastData>> {
    return delay({ code: 200, message: 'success', data: forecastPoints() })
  },
  parking(): Promise<ApiResult<UserParking[]>> {
    return delay({ code: 200, message: 'success', data: parkings.map((p) => ({ ...p })) })
  },
  shuttleRoutes(): Promise<ApiResult<ShuttleRouteGroup[]>> {
    return delay({ code: 200, message: 'success', data: routes.map((r) => ({ ...r, stations: [...r.stations] })) })
  },
  pois(category?: PoiCategory): Promise<ApiResult<Poi[]>> {
    const list = category ? pois.filter((p) => p.category === category) : pois
    return delay({ code: 200, message: 'success', data: list.map((p) => ({ ...p })) })
  },
  recommendRoutes(type: 'avoid' | 'classic'): Promise<ApiResult<RecommendData>> {
    return delay({ code: 200, message: 'success', data: recommend(type) })
  },
  spotDetail(spotId: string): Promise<ApiResult<SpotDetailData>> {
    const s = spots.find((x) => x.spotId === spotId)
    if (!s) {
      return delay({ code: 404, message: '景点不存在', data: null as any })
    }
    const brief = toBrief(s)
    const trend = Array.from({ length: 12 }, (_, i) => ({
      hour: 8 + i,
      visitors: Math.round(s.cap * s.base * (0.55 + 0.45 * Math.sin((i / 11) * Math.PI))),
    }))
    const detailPois = pois.map((p) => ({
      poiId: p.poiId,
      name: p.name,
      category: p.category,
      categoryText: p.categoryText,
      distanceM: Math.round(Math.hypot(p.locationX - s.x, p.locationY - s.y) * 20),
      latitude: p.latitude,
      longitude: p.longitude,
    }))
    detailPois.sort((a, b) => a.distanceM - b.distanceM)
    return delay({
      code: 200,
      message: 'success',
      data: { ...brief, currentVisitors: Math.round(s.cap * s.base), capacity: s.cap, trend, pois: detailPois },
    })
  },
}
