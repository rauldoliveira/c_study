import axios from 'axios'

export default {
    doAction (to, from, next) {
        
                axios.post('http://127.0.0.1:1080/api/v2/action', {"data":"..."})
                    .then(request => this.actionSuccessful(request))
                    .catch(() => this.actionFailed())
    },

    actionSuccessful (req) {
        ...
    },

    actionFailed () {
        ...
    }
}