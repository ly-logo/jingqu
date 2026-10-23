import type {
  ApiResult,
  Spot,
  ParkingLot,
  Shuttle,
  SystemStatus,
  LoginRequest,
  LoginResponse,
  FlowPoint,
  CongestionLevel,
} from './types'

function delay<T>(data: ApiResult<T>): Promise<ApiResult<T>> {
  return new Promise((resolve) => {
    setTimeout(() => resolve(data), 100 + Math.random() * 200)
  })
}

function getLevel(ratio: number): CongestionLevel {
  if (ratio < 0.3) return 'EMPTY'
  if (ratio < 0.6) return 'NORMAL'
  if (ratio < 0.9) return 'MODERATE'
  if (ratio < 1.0) return 'CROWDED'
  return 'OVERLOADED'
}

interface MockSpot {
  spotId: string
  name: string
  locationX: number
  locationY: number
  capacity: number
  dailyCapacity: number
  currentVisitors: number
  totalIn: number
  baseRatio: number
  history: FlowPoint[]
}

const spots: MockSpot[] = [
  { spotId: 'spot_01', name: '观瀑台', locationX: 22, locationY: 68, capacity: 800, dailyCapacity: 6000, currentVisitors: 0, totalIn: 0, baseRatio: 0.95, history: [] },
  { spotId: 'spot_02', name: '云海栈道', locationX: 48, locationY: 82, capacity: 900, dailyCapacity: 7200, currentVisitors: 0, totalIn: 0, baseRatio: 0.45, history: [] },
  { spotId: 'spot_03', name: '青石古镇', locationX: 75, locationY: 55, capacity: 850, dailyCapacity: 6800, currentVisitors: 0, totalIn: 0, baseRatio: 0.62, history: [] },
  { spotId: 'spot_04', name: '望湖亭', locationX: 60, locationY: 30, capacity: 400, dailyCapacity: 3200, currentVisitors: 0, totalIn: 0, baseRatio: 0.35, history: [] },
  { spotId: 'spot_05', name: '百花谷', locationX: 35, locationY: 40, capacity: 600, dailyCapacity: 4800, currentVisitors: 0, totalIn: 0, baseRatio: 0.5, history: [] },
  { spotId: 'spot_06', name: '索道站', locationX: 15, locationY: 25, capacity: 300, dailyCapacity: 2400, currentVisitors: 0, totalIn: 0, baseRatio: 0.75, history: [] },
  { spotId: 'spot_07', name: '竹林径', locationX: 82, locationY: 78, capacity: 500, dailyCapacity: 4000, currentVisitors: 0, totalIn: 0, baseRatio: 0.28, history: [] },
  { spotId: 'spot_08', name: '山顶广场', locationX: 50, locationY: 90, capacity: 1000, dailyCapacity: 8000, currentVisitors: 0, totalIn: 0, baseRatio: 0.4, history: [] },
]

for (const s of spots) {
  s.currentVisitors = Math.round(s.capacity * s.baseRatio * (0.8 + Math.random() * 0.4))
  s.totalIn = Math.round(s.dailyCapacity * 0.35 * (0.7 + Math.random() * 0.6))
}

const parkings: ParkingLot[] = [
  { parkingId: 'park_01', name: '东门停车场', totalSpaces: 500, occupiedSpaces: 320, freeSpaces: 180 },
  { parkingId: 'park_02', name: '西门停车场', totalSpaces: 300, occupiedSpaces: 210, freeSpaces: 90 },
  { parkingId: 'park_03', name: '南门停车场', totalSpaces: 200, occupiedSpaces: 80, freeSpaces: 120 },
]

const stations = ['东门', '索道站', '观瀑台', '百花谷', '望湖亭', '青石古镇', '山顶广场']
const shuttles: Shuttle[] = Array.from({ length: 15 }, (_, i) => {
  const rand = Math.random()
  const status: Shuttle['status'] = rand < 0.7 ? 'RUNNING' : rand < 0.9 ? 'IDLE' : 'MAINTENANCE'
  return {
    shuttleId: `shuttle_${String(i + 1).padStart(2, '0')}`,
    name: `摆渡车${String(i + 1).padStart(2, '0')}号`,
    capacity: 20,
    currentStation: stations[Math.floor(Math.random() * stations.length)],
    passengerCount: status === 'RUNNING' ? Math.floor(Math.random() * 21) : 0,
    status,
  }
})

function tickSpots() {
  for (const s of spots) {
    const change = (Math.random() - 0.5) * 2 * 0.03 * s.capacity
    let next = s.currentVisitors + change
    const anchor = s.capacity * s.baseRatio
    next += (anchor - next) * 0.05
    next = Math.max(0, Math.min(s.capacity * 1.15, next))
    const inFlow = Math.max(0, Math.round(change > 0 ? change : Math.random() * s.capacity * 0.01))
    s.totalIn += inFlow
    s.currentVisitors = Math.round(next)
    s.history.push({ timestamp: Date.now(), visitors: s.currentVisitors })
    if (s.history.length > 1200) s.history.shift()
  }
}

function tickParkings() {
  for (const p of parkings) {
    const change = Math.round((Math.random() - 0.5) * 2 * 0.03 * p.totalSpaces)
    p.occupiedSpaces = Math.max(0, Math.min(p.totalSpaces, p.occupiedSpaces + change))
    p.freeSpaces = p.totalSpaces - p.occupiedSpaces
  }
}

function tickShuttles() {
  for (const sh of shuttles) {
    if (Math.random() < 0.1) {
      sh.currentStation = stations[Math.floor(Math.random() * stations.length)]
    }
    if (sh.status === 'RUNNING') {
      sh.passengerCount = Math.max(0, Math.min(sh.capacity, sh.passengerCount + Math.round((Math.random() - 0.5) * 6)))
    }
    if (Math.random() < 0.05) {
      const rand = Math.random()
      sh.status = rand < 0.7 ? 'RUNNING' : rand < 0.9 ? 'IDLE' : 'MAINTENANCE'
    }
  }
}

function tickAll() {
  tickSpots()
  tickParkings()
  tickShuttles()
}

setInterval(tickAll, 1000)
tickAll()

function toSpot(s: MockSpot): Spot {
  const ratio = s.currentVisitors / s.capacity
  return {
    spotId: s.spotId,
    name: s.name,
    locationX: s.locationX,
    locationY: s.locationY,
    capacity: s.capacity,
    dailyCapacity: s.dailyCapacity,
    currentVisitors: s.currentVisitors,
    totalIn: s.totalIn,
    arrivalRate: Math.round(s.capacity * 0.02 * (0.5 + Math.random())),
    departureRate: Math.round(s.capacity * 0.018 * (0.5 + Math.random())),
    level: getLevel(ratio),
  }
}

export const mockAuth = {
  login(req: LoginRequest): Promise<ApiResult<LoginResponse>> {
    const { username, password } = req
    if (!username || !password) {
      return delay({ code: 400, message: '用户名或密码错误', data: null as any })
    }
    let role: LoginResponse['role'] = 'viewer'
    if (username === 'admin' && password === '123456') role = 'admin'
    else if (username === 'operator' && password === '123456') role = 'operator'
    return delay({
      code: 200,
      message: 'success',
      data: { token: `mock-${Date.now()}`, role, username },
    })
  },
  register(req: LoginRequest): Promise<ApiResult<LoginResponse>> {
    const { username, password } = req
    if (!/^[A-Za-z0-9_]{3,20}$/.test(username)) {
      return delay({ code: 400, message: '用户名需为 3~20 位字母、数字或下划线', data: null as any })
    }
    if (password.length < 6 || password.length > 32) {
      return delay({ code: 400, message: '密码长度需为 6~32 位', data: null as any })
    }
    return delay({
      code: 200,
      message: 'success',
      data: { token: `mock-${Date.now()}`, role: 'viewer', username },
    })
  },
}

export const mockSpot = {
  list(): Promise<ApiResult<Spot[]>> {
    return delay({ code: 200, message: 'success', data: spots.map(toSpot) })
  },
  flow(spotId: string): Promise<ApiResult<FlowPoint[]>> {
    const s = spots.find((x) => x.spotId === spotId)
    return delay({ code: 200, message: 'success', data: s ? [...s.history] : [] })
  },
  congestion(spotId: string): Promise<ApiResult<Spot>> {
    const s = spots.find((x) => x.spotId === spotId)
    return delay({ code: 200, message: 'success', data: s ? toSpot(s) : (null as any) })
  },
}

export const mockParking = {
  list(): Promise<ApiResult<ParkingLot[]>> {
    return delay({ code: 200, message: 'success', data: parkings.map((p) => ({ ...p })) })
  },
}

export const mockShuttle = {
  list(): Promise<ApiResult<Shuttle[]>> {
    return delay({ code: 200, message: 'success', data: shuttles.map((s) => ({ ...s })) })
  },
}

export const mockSystem = {
  status(): Promise<ApiResult<SystemStatus>> {
    const currentVisitors = spots.reduce((sum, s) => sum + s.currentVisitors, 0)
    const crowdedSpotCount = spots.filter((s) => {
      const ratio = s.currentVisitors / s.capacity
      return ratio >= 0.9
    }).length
    const freeSpaces = parkings.reduce((sum, p) => sum + p.freeSpaces, 0)
    const shuttleOnline = shuttles.filter((s) => s.status === 'RUNNING').length
    return delay({
      code: 200,
      message: 'success',
      data: {
        totalCapacity: 20000,
        currentVisitors,
        crowdedSpotCount,
        freeSpaces,
        shuttleOnline,
        shuttleTotal: shuttles.length,
      },
    })
  },
}
