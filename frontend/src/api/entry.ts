import http from './http'
import type { ApiResult, VisitorEntryRequest, ParkingEntryRequest } from './types'

export const entryApi = {
  submitVisitor(data: VisitorEntryRequest): Promise<ApiResult<null>> {
    return http.post('/entry/visitor', data).then((r) => r.data)
  },
  submitParking(data: ParkingEntryRequest): Promise<ApiResult<null>> {
    return http.post('/entry/parking', data).then((r) => r.data)
  },
}
