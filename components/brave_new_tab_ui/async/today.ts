import * as Background from '../../common/Background'
import AsyncActionHandler from '../../common/AsyncActionHandler'
// import * as Actions from '../actions/today_actions'
import { init } from '../actions/new_tab_actions'
import * as Actions from '../actions/today_actions'

import MessageTypes = Background.MessageTypes.Today
import Messages = BraveToday.Messages

const handler = new AsyncActionHandler()

handler.on(init.getType(), async (store, action) => {
  try {
    const data = await Background.send<Messages.GetFeedResponse>(MessageTypes.getFeed)
    console.log('got feed', data)
    store.dispatch(Actions.dataReceived(data))
  } catch (e) {
    console.error('error receiving feed', e)
    store.dispatch(Actions.errorGettingDataFromBackground(e))
  }
})


export default handler.middleware