import http from './http'
import type { ApiResult, ParkingLot } from './types'

export const parkingApi = {
  list(): Promise<ApiResult<ParkingLot[]>> {
    return http.get('/parking').then((r) => r.data)
  },
}
