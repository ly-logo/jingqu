import http from './http'
import type { ApiResult, Spot, FlowPoint } from './types'

export const spotApi = {
  list(): Promise<ApiResult<Spot[]>> {
    return http.get('/spots').then((r) => r.data)
  },
  flow(spotId: string): Promise<ApiResult<FlowPoint[]>> {
    return http.get(`/spots/${spotId}/flow`).then((r) => r.data)
  },
  congestion(spotId: string): Promise<ApiResult<Spot>> {
    return http.get(`/spots/${spotId}/congestion`).then((r) => r.data)
  },
}
