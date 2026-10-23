import http from './http'
import type { ApiResult, EventItem, EventStatus } from './types'

export const eventApi = {
  list(params?: { type?: string; level?: string; status?: string }): Promise<ApiResult<EventItem[]>> {
    return http.get('/events', { params }).then((r) => r.data)
  },
  updateStatus(eventId: number, status: EventStatus): Promise<ApiResult<null>> {
    return http.put(`/events/${eventId}/status`, { status }).then((r) => r.data)
  },
}
