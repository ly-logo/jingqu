import http from './http'
import type { ApiResult, Suggestion, SuggestionStatus } from './types'

export const schedulingApi = {
  list(): Promise<ApiResult<Suggestion[]>> {
    return http.get('/scheduling').then((r) => r.data)
  },
  detail(suggestionId: number): Promise<ApiResult<Suggestion>> {
    return http.get(`/scheduling/${suggestionId}`).then((r) => r.data)
  },
  updateStatus(suggestionId: number, status: SuggestionStatus): Promise<ApiResult<null>> {
    return http.put(`/scheduling/${suggestionId}/status`, { status }).then((r) => r.data)
  },
}
