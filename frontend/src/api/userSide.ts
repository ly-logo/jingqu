import http from './http'
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
} from './types'

/** 用户端聚合只读接口（后端 UserSideController） */
export const userSideApi = {
  overview(): Promise<ApiResult<OverviewData>> {
    return http.get('/user/overview').then((r) => r.data)
  },
  rankings(type: 'avoid' | 'crowded'): Promise<ApiResult<RankingsData>> {
    return http.get('/user/rankings', { params: { type } }).then((r) => r.data)
  },
  forecast(): Promise<ApiResult<ForecastData>> {
    return http.get('/user/forecast').then((r) => r.data)
  },
  parking(): Promise<ApiResult<UserParking[]>> {
    return http.get('/user/parking').then((r) => r.data)
  },
  shuttleRoutes(): Promise<ApiResult<ShuttleRouteGroup[]>> {
    return http.get('/user/shuttle-routes').then((r) => r.data)
  },
  pois(category?: PoiCategory): Promise<ApiResult<Poi[]>> {
    return http.get('/user/pois', { params: category ? { category } : {} }).then((r) => r.data)
  },
  recommendRoutes(type: 'avoid' | 'classic'): Promise<ApiResult<RecommendData>> {
    return http.get('/user/routes/recommend', { params: { type } }).then((r) => r.data)
  },
  spotDetail(spotId: string): Promise<ApiResult<SpotDetailData>> {
    return http.get(`/spots/${spotId}/detail`).then((r) => r.data)
  },
}
