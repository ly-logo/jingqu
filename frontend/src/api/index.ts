import { authApi } from './auth'
import { spotApi } from './spot'
import { parkingApi } from './parking'
import { shuttleApi } from './shuttle'
import { systemApi } from './system'
import { eventApi } from './event'
import { schedulingApi } from './scheduling'
import { ruleApi } from './rule'
import { userApi } from './user'
import { statisticsApi } from './statistics'
import { simulationApi } from './simulation'
import { monitorApi } from './monitor'
import { settingsApi } from './settings'
import { entryApi } from './entry'
import { userSideApi } from './userSide'
import { mockAuth, mockSpot, mockParking, mockShuttle, mockSystem } from './mock'
import { mockUserSide } from './mock-user'
import {
  mockEvent,
  mockScheduling,
  mockRule,
  mockUser,
  mockStatistics,
  mockSimulation,
  mockMonitor,
  mockSettings,
  mockEntry,
} from './mock-admin'
import type {
  LoginRequest,
  ApiResult,
  LoginResponse,
  Spot,
  FlowPoint,
  ParkingLot,
  Shuttle,
  SystemStatus,
  EventItem,
  EventStatus,
  Suggestion,
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
  OverviewData,
  RankingsData,
  ForecastData,
  UserParking,
  ShuttleRouteGroup,
  Poi,
  PoiCategory,
  RecommendData,
  SpotDetailData,
} from './types'

/**
 * Mock 开关：默认走真实 REST 接口；设置环境变量 VITE_USE_MOCK=true 可切回本地 Mock。
 * 两种模式下函数签名完全一致，页面逻辑无需改动。
 */
export const USE_MOCK = import.meta.env.VITE_USE_MOCK === 'true'

// ==================== 认证 ====================

export const login = (data: LoginRequest): Promise<ApiResult<LoginResponse>> =>
  USE_MOCK ? mockAuth.login(data) : authApi.login(data)

export const register = (data: LoginRequest): Promise<ApiResult<LoginResponse>> =>
  USE_MOCK ? mockAuth.register(data) : authApi.register(data)

// ==================== 景点 / 流量 ====================

export const getSpots = (): Promise<ApiResult<Spot[]>> =>
  USE_MOCK ? mockSpot.list() : spotApi.list()

export const getSpotFlow = (spotId: string): Promise<ApiResult<FlowPoint[]>> =>
  USE_MOCK ? mockSpot.flow(spotId) : spotApi.flow(spotId)

export const getSpotCongestion = (spotId: string): Promise<ApiResult<Spot>> =>
  USE_MOCK ? mockSpot.congestion(spotId) : spotApi.congestion(spotId)

// ==================== 资源 ====================

export const getParkings = (): Promise<ApiResult<ParkingLot[]>> =>
  USE_MOCK ? mockParking.list() : parkingApi.list()

export const getShuttles = (): Promise<ApiResult<Shuttle[]>> =>
  USE_MOCK ? mockShuttle.list() : shuttleApi.list()

// ==================== 系统状态 ====================

export const getSystemStatus = (): Promise<ApiResult<SystemStatus>> =>
  USE_MOCK ? mockSystem.status() : systemApi.status()

// ==================== 事件 / 告警 ====================

export const getEvents = (params?: {
  type?: string
  level?: string
  status?: string
}): Promise<ApiResult<EventItem[]>> => (USE_MOCK ? mockEvent.list(params) : eventApi.list(params))

export const updateEventStatus = (eventId: number, status: EventStatus): Promise<ApiResult<null>> =>
  USE_MOCK ? mockEvent.updateStatus(eventId, status) : eventApi.updateStatus(eventId, status)

// ==================== 调度建议 ====================

export const getSuggestions = (): Promise<ApiResult<Suggestion[]>> =>
  USE_MOCK ? mockScheduling.list() : schedulingApi.list()

export const getSuggestionDetail = (suggestionId: number): Promise<ApiResult<Suggestion>> =>
  USE_MOCK ? mockScheduling.detail(suggestionId) : schedulingApi.detail(suggestionId)

export const updateSuggestionStatus = (
  suggestionId: number,
  status: SuggestionStatus,
): Promise<ApiResult<null>> =>
  USE_MOCK
    ? mockScheduling.updateStatus(suggestionId, status)
    : schedulingApi.updateStatus(suggestionId, status)

// ==================== 规则配置 ====================

export const getRules = (): Promise<ApiResult<Rule[]>> =>
  USE_MOCK ? mockRule.list() : ruleApi.list()

export const createRule = (data: Omit<Rule, 'ruleId'>): Promise<ApiResult<Rule>> =>
  USE_MOCK ? mockRule.create(data) : ruleApi.create(data)

export const updateRule = (ruleId: number, data: Partial<Rule>): Promise<ApiResult<Rule>> =>
  USE_MOCK ? mockRule.update(ruleId, data) : ruleApi.update(ruleId, data)

export const deleteRule = (ruleId: number): Promise<ApiResult<null>> =>
  USE_MOCK ? mockRule.remove(ruleId) : ruleApi.remove(ruleId)

// ==================== 用户管理 ====================

export const getUsers = (): Promise<ApiResult<User[]>> =>
  USE_MOCK ? mockUser.list() : userApi.list()

export const createUser = (data: UserCreateRequest): Promise<ApiResult<User>> =>
  USE_MOCK ? mockUser.create(data) : userApi.create(data)

export const deleteUser = (userId: number): Promise<ApiResult<null>> =>
  USE_MOCK ? mockUser.remove(userId) : userApi.remove(userId)

// ==================== 数据统计 ====================

export const getStatistics = (range?: string): Promise<ApiResult<StatisticsData>> =>
  USE_MOCK ? mockStatistics.overview() : statisticsApi.overview(range)

// ==================== 模拟控制 ====================

export const getSimulationState = (): Promise<ApiResult<SimulationState>> =>
  USE_MOCK ? mockSimulation.state() : simulationApi.state()

export const startSimulation = (): Promise<ApiResult<SimulationState>> =>
  USE_MOCK ? mockSimulation.start() : simulationApi.start()

export const stopSimulation = (): Promise<ApiResult<SimulationState>> =>
  USE_MOCK ? mockSimulation.stop() : simulationApi.stop()

export const updateSimulationConfig = (
  config: Partial<SimulationConfig>,
): Promise<ApiResult<SimulationConfig>> =>
  USE_MOCK ? mockSimulation.updateConfig(config) : simulationApi.updateConfig(config)

// ==================== 系统监控 ====================

export const getMonitorStatus = (): Promise<ApiResult<SystemMonitor>> =>
  USE_MOCK ? mockMonitor.status() : monitorApi.status()

// ==================== 系统设置 ====================

export const getSettings = (): Promise<ApiResult<SystemSettings>> =>
  USE_MOCK ? mockSettings.get() : settingsApi.get()

export const updateSettings = (data: Partial<SystemSettings>): Promise<ApiResult<SystemSettings>> =>
  USE_MOCK ? mockSettings.update(data) : settingsApi.update(data)

// ==================== 数据录入 ====================

export const submitVisitorEntry = (data: VisitorEntryRequest): Promise<ApiResult<null>> =>
  USE_MOCK ? mockEntry.submitVisitor(data) : entryApi.submitVisitor(data)

export const submitParkingEntry = (data: ParkingEntryRequest): Promise<ApiResult<null>> =>
  USE_MOCK ? mockEntry.submitParking(data) : entryApi.submitParking(data)

// ==================== 用户端（/api/user/*） ====================

export const getUserOverview = (): Promise<ApiResult<OverviewData>> =>
  USE_MOCK ? mockUserSide.overview() : userSideApi.overview()

export const getUserRankings = (type: 'avoid' | 'crowded'): Promise<ApiResult<RankingsData>> =>
  USE_MOCK ? mockUserSide.rankings(type) : userSideApi.rankings(type)

export const getUserForecast = (): Promise<ApiResult<ForecastData>> =>
  USE_MOCK ? mockUserSide.forecast() : userSideApi.forecast()

export const getUserParking = (): Promise<ApiResult<UserParking[]>> =>
  USE_MOCK ? mockUserSide.parking() : userSideApi.parking()

export const getUserShuttleRoutes = (): Promise<ApiResult<ShuttleRouteGroup[]>> =>
  USE_MOCK ? mockUserSide.shuttleRoutes() : userSideApi.shuttleRoutes()

export const getUserPois = (category?: PoiCategory): Promise<ApiResult<Poi[]>> =>
  USE_MOCK ? mockUserSide.pois(category) : userSideApi.pois(category)

export const getUserRecommendRoutes = (type: 'avoid' | 'classic'): Promise<ApiResult<RecommendData>> =>
  USE_MOCK ? mockUserSide.recommendRoutes(type) : userSideApi.recommendRoutes(type)

export const getSpotDetail = (spotId: string): Promise<ApiResult<SpotDetailData>> =>
  USE_MOCK ? mockUserSide.spotDetail(spotId) : userSideApi.spotDetail(spotId)

export * from './types'
